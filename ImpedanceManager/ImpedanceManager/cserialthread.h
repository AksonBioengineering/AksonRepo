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
          e_getFirmwareID           = 0x01
        , e_takeMeasEis             = 0x02
        , e_giveMeasChunkEis        = 0x03
        , e_endMeasEis              = 0x04
        , e_takeMeasCv              = 0x05
        , e_giveMeasChunkCv         = 0x06
        , e_endMeasCv               = 0x07
    };

    typedef struct
    {
        quint8 m_syncByte;
        ESerialCommand_t m_command;
        quint32 m_length;
        QByteArray m_data;
        qint16 m_crc;
    } ESerialFrame_t;

    explicit CSerialThread(const QString& port, QObject *parent = 0);
    ~CSerialThread();

    void run(); // inherited

signals:
    void openPort(const int& val);
    void rxTimeout(const int&);

    // frames (IM- impredance manager, ES- embedded system):            // Sender:
    void received_getFirmwareID(const MeasureUtility::union32_t& id);   // IM

    // EIS
    void received_takeMeasEis(const bool& ack);                         // IM
    void received_giveMeasChunkEis(const union32_t&, const union32_t&,
                                   const union32_t&);                   // ES
    void received_endMeasEis();                                         // ES

    // CV
    void received_takeMeasCv(const bool& ack);                          // IM
    void received_giveMeasChunkCv(const quint16&, const union32_t&,
                             const union32_t&);                         // ES
    void received_endMeasCv();                                          // ES

public slots:
    void on_readyRead();
    void on_closePort();

    // frames:
    void on_rxTimeout();
    void on_send_getFirmwareID();
    void on_send_takeMeasEis(const quint8&,
                             const union32_t&,
                             const union32_t&,
                             const qint16&,
                             const quint8&);

    void on_send_takeMeasCv( const qint16&,
                             const qint16&,
                             const quint8&,
                             const qint16&,
                             const qint16&);

private:
    qint16 getCrc(const QByteArray& bArray);
    qint16 getCrc(const ESerialFrame_t& frame);
    int digForFrames(QByteArray& buffer);
    void sendData(const ESerialCommand_t& command,
                  const QByteArray& data,
                  const bool wantAck);
    void frameReady();

    // frames
    void send_endMeasEis();
    void send_endMeasCv();

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
