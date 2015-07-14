#ifndef CSETTINGSDIALOG_H
#define CSETTINGSDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPortInfo>
#include <QSerialPort>
#include <QByteArray>

#include "cserialthread.h"

namespace Ui {
class CSettingsDialog;
}

class CSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CSettingsDialog(QWidget *parent = 0);
    ~CSettingsDialog();

signals:
    void sendData(const quint8& command, const QByteArray& data);

private slots:
    void on_pbSerialCheck_clicked();

private:
    Ui::CSettingsDialog *ui;
    CSerialThread* mp_serialThread;

    void initComponents();
};

#endif // CSETTINGSDIALOG_H
