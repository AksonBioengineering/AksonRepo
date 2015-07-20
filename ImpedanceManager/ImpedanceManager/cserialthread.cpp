#include "cserialthread.h"


CSerialThread::CSerialThread(const QString& port, QObject *parent) :
    QThread(parent)
{
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
}

CSerialThread::~CSerialThread()
{   
    if (mp_frameStruct) delete mp_frameStruct;
    if (mp_RxTimeoutTimer) delete mp_RxTimeoutTimer;

    if (mp_serial)
    {
        mp_serial->close();
        //delete mp_serial;
    }
}

void CSerialThread::run()
{
    if (!mp_serial->open(QIODevice::ReadWrite))
    {
        emit openPort(1);
        qWarning() << "Serial port" << mp_serial->portName() << "is busy";
        exit(1);
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

    emit openPort(0);

    mp_serial->setBaudRate(QSerialPort::Baud115200);
    mp_serial->setDataBits(QSerialPort::Data8);
    mp_serial->setParity(QSerialPort::NoParity);
    mp_serial->setStopBits(QSerialPort::OneStop);
    mp_serial->setFlowControl(QSerialPort::NoFlowControl);

    exec();
}

void CSerialThread::sendData(const ESerialCommand_t& command, const QByteArray& data)
{
    quint16 crc = 0;
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
        mp_RxTimeoutTimer->start();
}

quint16 CSerialThread::getCrc(const QByteArray& bArray)
{
    quint16 crc = 0;

    for (auto item : bArray)
        crc += item;

    return ~crc;
}

quint16 CSerialThread::getCrc(const ESerialFrame_t& frame)
{
    quint16 crc = 0;

    crc += frame.m_syncByte;
    crc += (quint8)frame.m_command;
    crc += frame.m_length;

    for (auto item : frame.m_data)
        crc += item;

    return ~crc;
}

void CSerialThread::on_readyRead()
{
    mp_RxTimeoutTimer->start(); // reset timer
    QByteArray receiveBuffer(mp_serial->readAll());

    while(receiveBuffer.length())
    {
        if (digForFrames(receiveBuffer))
        {
            qWarning() << "Examinating bytes from tcp failed";
        }
    }

    // no need to emit if more than 1 item in queue
    if (m_frameQueue.length() == 1)
        frameReady();
}

int CSerialThread::digForFrames(QByteArray& buffer)
{
    static quint32 currentIndex = 0;
    static quint8 data = 0;
    static bool newFrame = false;
    static quint32 lastLen = 2 + sizeof(quint32);

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

                quint16 calculatedCrc = getCrc(*mp_frameStruct);
                if (mp_frameStruct->m_crc == calculatedCrc)
                    m_frameQueue.enqueue(mp_frameStruct);
                else
                    qWarning() << "Bad crc. Received:" << mp_frameStruct->m_crc
                               << "Calculated:" << calculatedCrc;

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

            case ESerialCommand_t::e_takeMeasEis: // answer
            {
                emit received_takeMeasEis((bool)frame.m_data[0]);
                break;
            }

            case ESerialCommand_t::e_giveMeasChunkEis: // command
            {
                union32_t realImp;
                union32_t imagImp;
                union32_t freqPoint;
                quint32 i = 0;

                for (; i < sizeof(union32_t); i++)
                    realImp.id8[i] = frame.m_data[i];

                for (; i < sizeof(union32_t); i++)
                    imagImp.id8[i] = frame.m_data[i];

                for (; i < sizeof(union32_t); i++)
                    freqPoint.id8[i] = frame.m_data[i];

                emit received_giveMeasChunkEis(realImp, imagImp, freqPoint);
                break;
            }

            case ESerialCommand_t::e_endMeasEis: // command
            {
                send_endMeasEis();
                emit received_endMeasEis();
                break;
            }

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
    sendData(ESerialCommand_t::e_getFirmwareID, sendArr);
}

void CSerialThread::on_send_takeMeasEis(const quint8& amplitude,
                                        const quint32& freqStart,
                                        const quint32& freqEnd,
                                        const quint16& nrOfSteps,
                                        const MeasureUtility::EStepType_t& stepType)
{
    QByteArray sendArr;
    sendArr.append(amplitude);

    for (quint32 i = 0; i < sizeof(quint32); i++)
        sendArr.append((quint8)(freqStart >> (i * 8)) & 0xFF);

    for (quint32 i = 0; i < sizeof(quint32); i++)
        sendArr.append((quint8)(freqEnd >> (i * 8)) & 0xFF);

    for (quint16 i = 0; i < sizeof(quint16); i++)
        sendArr.append((quint8)(nrOfSteps >> (i * 8)) & 0xFF);

    sendArr.append((quint8)stepType);
    sendData(ESerialCommand_t::e_takeMeasEis, sendArr);
}

void CSerialThread::send_endMeasEis()
{
    QByteArray sendArr;
    sendData(ESerialCommand_t::e_endMeasEis, sendArr);
}















