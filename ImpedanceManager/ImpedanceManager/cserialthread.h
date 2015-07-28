#ifndef CSERIALTHREAD_H
#define CSERIALTHREAD_H

#include <QThread>
#include <QtSerialPort/QSerialPortInfo>
#include <QSerialPort>
#include <QString>
#include <QDebug>
#include <QByteArray>
#include <QQueue>
#include <QTimer>

#include "MeasureUtility.h"

using namespace MeasureUtility;

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
    void rxTimeout(const int&);

    // frames:                                                              // Sender:
    void received_getFirmwareID(const MeasureUtility::union32_t& id);   // IM
    void received_takeMeasEis(const bool& ack);                         // IM
    void received_giveMeasChunkEis(const union32_t&, const union32_t&, const union32_t&); // ES

    void received_endMeasEis();                                         // ES

public slots:
    void on_readyRead();
    void on_closePort();

    // frames:
    void on_rxTimeout();
    void on_send_getFirmwareID();
    void on_send_takeMeasEis(const quint8&,
                             const union32_t&,
                             const union32_t&,
                             const quint16&,
                             const quint8&);

private:
    quint16 getCrc(const QByteArray& bArray);
    quint16 getCrc(const ESerialFrame_t& frame);
    int digForFrames(QByteArray& buffer);
    void sendData(const ESerialCommand_t& command,
                  const QByteArray& data,
                  const bool wantAck);
    void frameReady();

    // frames
    void send_endMeasEis();

    QSerialPort* mp_serial;
    QByteArray m_sendBuffer;
    QQueue<ESerialFrame_t*> m_frameQueue;
    ESerialFrame_t* mp_frameStruct;
    ESerialCommand_t m_sentCommand;
    QTimer* mp_RxTimeoutTimer;

    static const quint8 m_syncByte = '?';
    static const int m_rxTimeoutInterval_ms = 5000;
};

#endif // CSERIALTHREAD_H
