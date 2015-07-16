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

    // Connections
    connect(mp_serial, SIGNAL(readyRead()),
            this, SLOT(on_readyRead()));

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
            case ESerialCommand_t::e_getFirmwareID:
            {
                MeasureUtility::Uint32Union_t id;
                id.id32 = 0;
                for (quint32 i = 0; i < sizeof(quint32); i++)
                    id.id8[i] = frame.m_data[i];

                emit received_getFirmwareID(id);
                break;
            }

            default:
            {
                qCritical() << "Unknown command received with code" << (int)frame.m_command
                            << "and length" << frame.m_length;
            }
        }

        /*qDebug() << "Command" << (int)frame.m_command;
        qDebug() << "Length" << frame.m_length;

        QString dataStr;
        for (auto item : frame.m_data)
            dataStr += QString("%1").arg((int)item) + " ";

        qDebug() << "Data" << dataStr;
        qDebug() << "Crc" << frame.m_crc;*/
    }
    while(m_frameQueue.length());
}

void CSerialThread::on_closePort()
{
    mp_serial->close();
    exit(0);
}

void CSerialThread::on_send_getFirmwareID()
{
    QByteArray emptyArr;
    sendData(ESerialCommand_t::e_getFirmwareID, emptyArr);
}
