#ifndef CSETTINGSDIALOG_H
#define CSETTINGSDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPortInfo>
#include <QSerialPort>
#include <QByteArray>

#include "cserialthread.h"
#include "MeasureUtility.h"
#include "csettingsmanager.h"

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
    void send_getFirmwareID();

private slots:
    void on_pbSerialCheck_clicked();

    void at_mp_SerialThread_openPort(const int&);
    void at_mp_SerialThread_rxTimeout(const int&);
    void at_received_getFirmwareID(const MeasureUtility::union32_t&);

private:
    Ui::CSettingsDialog *ui;
    CSerialThread* mp_serialThread;

    void initComponents();
};

#endif // CSETTINGSDIALOG_H
