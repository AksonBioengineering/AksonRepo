#include "cserialthread.h"


CSerialThread::CSerialThread(const QString& port, QObject *parent) :
    QThread(parent)
{
    //Q_ASSERT(parent);

    mp_serial = new QSerialPort(this);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (info.portName() == port)
        {
            mp_serial->setPortName(port);
            break;
        }
    }

    if (mp_serial->portName() != port)
        qWarning() << "Serial port" << port << "not found!";

    // just to mark the last used port name
    mp_serial->setPortName(port);
}

CSerialThread::~CSerialThread()
{   
    if (mp_frameStruct) delete mp_frameStruct;
    if (mp_RxTimeoutTimer) delete mp_RxTimeoutTimer;

    if (mp_serial)
    {
        mp_serial->close();
        mp_serial->deleteLater();
    }
}

void CSerialThread::run()
{
    if (!mp_serial->open(QIODevice::ReadWrite))
    {
        emit openPort(1);
        qWarning() << "Cannot open serial port" << mp_serial->portName();
        exit(1);
        return;
    }

    mp_RxTimeoutTimer = new QTimer();

    // Connections

    // com port
    connect(mp_serial, SIGNAL(readyRead()),
            this, SLOT(on_readyRead()), Qt::UniqueConnection);

    // Timer
    connect(mp_RxTimeoutTimer, SIGNAL(timeout()),
            this, SLOT(on_rxTimeout()), Qt::UniqueConnection);

    mp_RxTimeoutTimer->setSingleShot(true);
    mp_RxTimeoutTimer->setInterval(m_rxTimeoutInterval_ms);

    mp_serial->setBaudRate(QSerialPort::Baud57600);
    mp_serial->setDataBits(QSerialPort::Data8);
    mp_serial->setParity(QSerialPort::NoParity);
    mp_serial->setStopBits(QSerialPort::OneStop);
    mp_serial->setFlowControl(QSerialPort::NoFlowControl);

    emit openPort(0);
    exec();
}

void CSerialThread::sendData(const ESerialCommand_t& command,
                             const QByteArray& data, const bool wantAck)
{
    qint16 crc = 0;
    quint32 len = 0;

    m_sendBuffer.clear();
    m_sendBuffer.append(m_syncByte);                      // sync byte
    m_sendBuffer.append((quint8)command);                 // command
    m_sentCommand = command;                              // timeout purposes

    len = data.length() + sizeof(crc);                    // len = data + crc
    m_sendBuffer.append((quint8)(len & 0xFF));            // len
    m_sendBuffer.append((quint8)((len >> 8) & 0xFF));     // len
    m_sendBuffer.append((quint8)((len >> 16) & 0xFF));    // len
    m_sendBuffer.append((quint8)((len >> 24) & 0xFF));    // len
    m_sendBuffer.append(data);                            // data

    crc = getCrc(m_sendBuffer);
    m_sendBuffer.append((quint8)(crc & 0xFF));            // crc
    m_sendBuffer.append((quint8)((crc >> 8) & 0xFF));     // crc

    if (!mp_serial->write(m_sendBuffer))
        qWarning() << "Sending data on port" << mp_serial->portName() << "failed";
    else
    {
        if (wantAck)
            mp_RxTimeoutTimer->start();
    }
}

qint16 CSerialThread::getCrc(const QByteArray& bArray)
{
    qint16 crc = 0;

    for (auto item : bArray)
        crc += (qint16)((quint8)item);

    return ~crc;
}

qint16 CSerialThread::getCrc(const ESerialFrame_t& frame)
{
    quint32 crc = 0;

    crc += frame.m_syncByte;
    crc += (qint16)((quint8)frame.m_command);
    crc += (qint16)frame.m_length;

    for (auto item : frame.m_data)
        crc += (qint16)((quint8)item);

    return ~crc;
}

void CSerialThread::on_readyRead()
{
    mp_RxTimeoutTimer->start(); // reset timer
    QByteArray receiveBuffer(mp_serial->readAll());
    int errorCode = 0;

    while(receiveBuffer.length())
    {
        errorCode = digForFrames(receiveBuffer);
        if (errorCode)
            qWarning() << "Examinating bytes from serial port failed" << errorCode;
    }

    if (m_frameQueue.length() >= 1)
        frameReady();
}

int CSerialThread::digForFrames(QByteArray& buffer)
{
    static thread_local quint32 currentIndex = 0;
    static thread_local bool newFrame = false;
    static thread_local quint32 lastLen = 2 + sizeof(quint32);
    quint8 data = 0;

    if (0 == buffer.length())
        return 3;

    data = (quint8)buffer[0];
    buffer.remove(0, 1);

    if (0 == currentIndex)
    {
        if (m_syncByte == data)
        {
            mp_frameStruct = new ESerialFrame_t;
            mp_frameStruct->m_length = 0;

            mp_frameStruct->m_syncByte = data;
            newFrame = true;
        }
        else return 1;
    }
    else if (newFrame)
    {
        if (1 == currentIndex)
            mp_frameStruct->m_command = (ESerialCommand_t)data;
        else if ((currentIndex > 1) && (currentIndex < lastLen))
            mp_frameStruct->m_length |= (data << (8 * (currentIndex - 2)));
        else
        {
            if ((currentIndex - lastLen) < (mp_frameStruct->m_length - 1))
            {
                if ((currentIndex - lastLen) == (mp_frameStruct->m_length - 2))
                    mp_frameStruct->m_crc = data;
                else
                    mp_frameStruct->m_data.append(data);
            }
            else if ((currentIndex - lastLen) == (mp_frameStruct->m_length - 1)) // last byte
            {
                mp_frameStruct->m_crc |= (data << 8);

                qint16 calculatedCrc = getCrc(*mp_frameStruct);
                if (mp_frameStruct->m_crc == calculatedCrc)
                    m_frameQueue.enqueue(mp_frameStruct);
                else
                {
                    qWarning("Bad CRC. Received 0x%X Calculated 0x%X",
                             mp_frameStruct->m_crc, calculatedCrc);
                    delete mp_frameStruct;
                }

                currentIndex = 0;
                return 0;
            }
        }
    }
    else return 2;

    currentIndex++;
    return 0;
}

void CSerialThread::frameReady()
{
    mp_RxTimeoutTimer->stop();
    static ESerialFrame_t frame;

    do
    {
        frame = *m_frameQueue.head();
        delete m_frameQueue.head();
        m_frameQueue.dequeue();

        m_sendBuffer.clear();
        m_sendBuffer.append(m_syncByte);
        m_sendBuffer.append((quint8)frame.m_command);

        switch (frame.m_command)
        {
            case ESerialCommand_t::e_getFirmwareID: // answer
            {
                MeasureUtility::union32_t id;
                id.id32 = 0;
                for (quint32 i = 0; i < sizeof(quint32); i++)
                    id.id8[i] = frame.m_data[i];

                emit received_getFirmwareID(id);
                break;
            }

            // EIS

            case ESerialCommand_t::e_takeMeasEis: // answer
            {
                qDebug() << "SERIAL: Answer for e_takeMeasEis";
                emit received_takeMeasEis((bool)frame.m_data[0]);
                break;
            }

            case ESerialCommand_t::e_giveMeasChunkEis: // command
            {
                union32_t realImp;
                union32_t imagImp;
                union32_t freqPoint;
                quint32 i = 0;

                for (size_t k = 0; k < sizeof(union32_t); i++, k++)
                    realImp.id8[k] = frame.m_data[i];

                for (size_t k = 0; k < sizeof(union32_t); i++, k++)
                    imagImp.id8[k] = frame.m_data[i];

                for (size_t k = 0; k < sizeof(union32_t); i++, k++)
                    freqPoint.id8[k] = frame.m_data[i];

                emit received_giveMeasChunkEis(realImp, imagImp, freqPoint);
                break;
            }

            case ESerialCommand_t::e_endMeasEis: // command
            {
                send_endMeasEis();
                emit received_endMeasEis();
                break;
            }

            // CV

            case ESerialCommand_t::e_takeMeasCv: // answer
            {
                qDebug() << "SERIAL: Answer for e_takeMeasCv";
                emit received_takeMeasCv((bool)frame.m_data[0]);
                break;
            }

            case ESerialCommand_t::e_giveMeasChunkCv: // command
            {
                quint16   sampleNr = 0;
                union32_t current;
                union32_t voltage;
                quint32 i = 0;

                for (size_t k = 0; k < sizeof(qint16); i++, k++)
                    sampleNr |= ((quint16)((quint8)frame.m_data[i])) << (k * 8);

                for (size_t k = 0; k < sizeof(union32_t); i++, k++)
                    current.id8[k] = frame.m_data[i];

                for (size_t k = 0; k < sizeof(union32_t); i++, k++)
                    voltage.id8[k] = frame.m_data[i];

                emit received_giveMeasChunkCv(sampleNr, current, voltage);
                break;
            }

            case ESerialCommand_t::e_endMeasCv: // command
            {
                send_endMeasCv();
                emit received_endMeasCv();
                break;
            }

            // CA

            case ESerialCommand_t::e_takeMeasCa: // answer
            {
                qDebug() << "SERIAL: Answer for e_takeMeasCa";
                emit received_takeMeasCa((bool)frame.m_data[0]);
                break;
            }

            case ESerialCommand_t::e_giveMeasChunkCa: // command
            {
                union32_t current;
                union32_t time;
                quint32 i = 0;

                for (size_t k = 0; k < sizeof(union32_t); i++, k++)
                    current.id8[k] = frame.m_data[i];

                for (size_t k = 0; k < sizeof(union32_t); i++, k++)
                    time.id8[k] = frame.m_data[i];

                emit received_giveMeasChunkCa(current, time);
                break;
            }

            case ESerialCommand_t::e_endMeasCa: // command
            {
                send_endMeasCa();
                emit received_endMeasCa();
                break;
            }

            // DPV

            case ESerialCommand_t::e_takeMeasDpv: // answer
            {
                qDebug() << "SERIAL: Answer for e_takeMeasDpv";
                emit received_takeMeasDpv((bool)frame.m_data[0]);
                break;
            }

            case ESerialCommand_t::e_giveMeasChunkDpv: // command
            {
                union32_t current;
                union32_t voltage;
                quint32 i = 0;

                for (size_t k = 0; k < sizeof(union32_t); i++, k++)
                    current.id8[k] = frame.m_data[i];

                for (size_t k = 0; k < sizeof(union32_t); i++, k++)
                    voltage.id8[k] = frame.m_data[i];

                emit received_giveMeasChunkDpv(current, voltage);
                break;
            }

            case ESerialCommand_t::e_endMeasDpv: // command
            {
                send_endMeasDpv();
                emit received_endMeasDpv();
                break;
            }

            // UNKNOWN

            default:
            {
                qCritical() << "Unknown command received with code" << (int)frame.m_command
                            << "and length" << frame.m_length;
            }
        }
    }
    while(m_frameQueue.length());
}

void CSerialThread::on_closePort()
{
    mp_serial->close();
    exit(0);
}

void CSerialThread::on_rxTimeout()
{
    qWarning() << "Rx timeout on command" << (int)m_sentCommand;
    emit rxTimeout((int)m_sentCommand);
    on_closePort();
}

void CSerialThread::on_send_getFirmwareID()
{
    QByteArray sendArr;
    sendData(ESerialCommand_t::e_getFirmwareID, sendArr, true);
}

void CSerialThread::on_send_takeMeasEis(const quint8& amplitude,
                                        const union32_t& freqStart,
                                        const union32_t& freqEnd,
                                        const qint16& nrOfSteps,
                                        const quint8& stepType)
{
    QByteArray sendArr;
    sendArr.append(amplitude);

    for (quint32 i = 0; i < sizeof(quint32); i++)
        sendArr.append(freqStart.id8[i]);

    for (quint32 i = 0; i < sizeof(quint32); i++)
        sendArr.append(freqEnd.id8[i]);

    for (quint32 i = 0; i < sizeof(qint16); i++)
        sendArr.append((quint8)(nrOfSteps >> (i * 8)) & 0xFF);

    sendArr.append((quint8)stepType);
    sendData(ESerialCommand_t::e_takeMeasEis, sendArr, true);
}

void CSerialThread::on_send_takeMeasCv( const qint16& potStart,
                                        const qint16& potEnd,
                                        const quint8&  nrOfCycles,
                                        const qint16& potStep,
                                        const qint16& scanDelay)
{
    QByteArray sendArr;
    for (quint32 i = 0; i < sizeof(qint16); i++)
        sendArr.append((quint8)(potStart >> (i * 8)) & 0xFF);

    for (quint32 i = 0; i < sizeof(qint16); i++)
        sendArr.append((quint8)(potEnd >> (i * 8)) & 0xFF);

    sendArr.append(nrOfCycles);

    for (quint32 i = 0; i < sizeof(qint16); i++)
        sendArr.append((quint8)(potStep >> (i * 8)) & 0xFF);

    for (quint32 i = 0; i < sizeof(qint16); i++)
            sendArr.append((quint8)(scanDelay >> (i * 8)) & 0xFF);

    sendData(ESerialCommand_t::e_takeMeasCv, sendArr, true);
}

void CSerialThread::on_send_takeMeasCa( const qint16& potential,
                                        const quint16& measTime,
                                        const union32_t& dt)
{
    QByteArray sendArr;
    for (quint32 i = 0; i < sizeof(qint16); i++)
        sendArr.append((quint8)(potential >> (i * 8)) & 0xFF);

    for (quint32 i = 0; i < sizeof(quint16); i++)
        sendArr.append((quint8)(measTime >> (i * 8)) & 0xFF);

    for (quint32 i = 0; i < sizeof(union32_t); i++)
        sendArr.append(dt.id8[i]);

    sendData(ESerialCommand_t::e_takeMeasCa, sendArr, true);
}

void CSerialThread::on_send_takeMeasDpv( const qint16&  qp,
                                         const quint16& qt,
                                         const quint32& pn,
                                         const quint16& pa,
                                         const quint16& pp,
                                         const quint16& pw,
                                         const qint16&  ps)
{
    QByteArray sendArr;
    for (quint32 i = 0; i < sizeof(qint16); i++)
        sendArr.append((quint8)(qp >> (i * 8)) & 0xFF);

    for (quint32 i = 0; i < sizeof(quint16); i++)
        sendArr.append((quint8)(qt >> (i * 8)) & 0xFF);

    for (quint32 i = 0; i < sizeof(quint32); i++)
        sendArr.append((quint8)(pn >> (i * 8)) & 0xFF);

    for (quint32 i = 0; i < sizeof(quint16); i++)
        sendArr.append((quint8)(pa >> (i * 8)) & 0xFF);

    for (quint32 i = 0; i < sizeof(quint16); i++)
        sendArr.append((quint8)(pp >> (i * 8)) & 0xFF);

    for (quint32 i = 0; i < sizeof(quint16); i++)
        sendArr.append((quint8)(pw >> (i * 8)) & 0xFF);

    for (quint32 i = 0; i < sizeof(qint16); i++)
        sendArr.append((quint8)(ps >> (i * 8)) & 0xFF);

    sendData(ESerialCommand_t::e_takeMeasDpv, sendArr, true);
}

void CSerialThread::send_endMeasEis()
{
    QByteArray sendArr;
    sendData(ESerialCommand_t::e_endMeasEis, sendArr, false);
}

void CSerialThread::send_endMeasCv()
{
    QByteArray sendArr;
    sendData(ESerialCommand_t::e_endMeasCv, sendArr, false);
}

void CSerialThread::send_endMeasCa()
{
    QByteArray sendArr;
    sendData(ESerialCommand_t::e_endMeasCa, sendArr, false);
}

void CSerialThread::send_endMeasDpv()
{
    QByteArray sendArr;
    sendData(ESerialCommand_t::e_endMeasDpv, sendArr, false);
}

void CSerialThread::updateSerialPort(const QString& port)
{
    mp_serial->setPortName(port);
}















