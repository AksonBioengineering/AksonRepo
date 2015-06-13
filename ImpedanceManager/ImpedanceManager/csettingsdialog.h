#ifndef CSETTINGSDIALOG_H
#define CSETTINGSDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPortInfo>
#include <QSerialPort>

namespace Ui {
class CSettingsDialog;
}

class CSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CSettingsDialog(QWidget *parent = 0);
    ~CSettingsDialog();

private slots:
    void on_pbSerialCheck_clicked();
    void on_bytesWritten(qint64 nrOfBytes);

private:
    Ui::CSettingsDialog *ui;
    QSerialPort* m_serial;

    void initComponents();
};

#endif // CSETTINGSDIALOG_H
