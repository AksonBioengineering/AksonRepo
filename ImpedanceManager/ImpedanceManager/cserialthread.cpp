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

    emit openPort(0);

    mp_serial->setBaudRate(QSerialPort::Baud115200);
    mp_serial->setDataBits(QSerialPort::Data8);
    mp_serial->setParity(QSerialPort::NoParity);
    mp_serial->setStopBits(QSerialPort::OneStop);
    mp_serial->setFlowControl(QSerialPort::NoFlowControl);

    exec();
}

void CSerialThread::on_sendData(const quint8& command, const QByteArray& data)
{
    quint16 crc = 0;
    quint32 len = 0;

    m_sendBuffer.clear();
    m_sendBuffer.append('?');                             // sync byte
    m_sendBuffer.append(command);                         // command

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
    quint16 crc;

    for (auto item : bArray)
        crc += item;

    return ~crc;
}
