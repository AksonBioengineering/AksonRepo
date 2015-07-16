#ifndef CSERIALTHREAD_H
#define CSERIALTHREAD_H

#include <QThread>
#include <QtSerialPort/QSerialPortInfo>
#include <QSerialPort>
#include <QString>
#include <QDebug>
#include <QByteArray>
#include <QQueue>

#include "MeasureUtility.h"

class CSerialThread : public QThread
{
    Q_OBJECT
public:
    enum class ESerialCommand_t
    {
          e_getFirmwareID         = 0x01
        , e_takeMeasEis           = 0x02
        , e_giveMeasChunkEis      = 0x03
        , e_endMeasEis            = 0x04
    };

    typedef struct
    {
        quint8 m_syncByte;
        ESerialCommand_t m_command;
        quint32 m_length;
        QByteArray m_data;
        quint16 m_crc;
    } ESerialFrame_t;

    explicit CSerialThread(const QString& port, QObject *parent = 0);
    ~CSerialThread();

    void run(); // inherited

signals:
    void openPort(const int& val);

    // frames:
    void received_getFirmwareID(const MeasureUtility::Uint32Union_t& id);

public slots:
    void on_readyRead();
    void frameReady();
    void on_closePort();

    // frames:
    void on_send_getFirmwareID();

private:
    quint16 getCrc(const QByteArray& bArray);
    quint16 getCrc(const ESerialFrame_t& frame);
    int digForFrames(QByteArray& buffer);
    void sendData(const ESerialCommand_t& command, const QByteArray& data);

    QSerialPort* mp_serial;
    QByteArray m_sendBuffer;
    QQueue<ESerialFrame_t*> m_frameQueue;
    ESerialFrame_t* mp_frameStruct;

    static const quint8 m_syncByte = '?';
};

#endif // CSERIALTHREAD_H
