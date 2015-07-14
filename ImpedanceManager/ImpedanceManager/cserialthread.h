#ifndef CSERIALTHREAD_H
#define CSERIALTHREAD_H

#include <QThread>
#include <QtSerialPort/QSerialPortInfo>
#include <QSerialPort>
#include <QString>
#include <QDebug>
#include <QByteArray>

class CSerialThread : public QThread
{
    Q_OBJECT
public:
    explicit CSerialThread(const QString& port, QObject *parent = 0);
    ~CSerialThread();

    void run(); // inherited

signals:
    void openPort(const int& val);

public slots:
    void on_sendData(const quint8& command, const QByteArray& data);

private:
    quint16 getCrc(const QByteArray& bArray);

    QSerialPort* mp_serial;
    QByteArray m_sendBuffer;

};

#endif // CSERIALTHREAD_H
