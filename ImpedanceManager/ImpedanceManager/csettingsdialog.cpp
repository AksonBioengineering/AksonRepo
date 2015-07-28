#include <QMessageBox>
#include <QDebug>

#include "csettingsdialog.h"
#include "ui_csettingsdialog.h"

CSettingsDialog::CSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSettingsDialog)
{
    ui->setupUi(this);
    initComponents();
}

CSettingsDialog::~CSettingsDialog()
{
    delete ui;

    if (mp_serialThread)
    {
        mp_serialThread->quit();
        mp_serialThread->wait();
        mp_serialThread->deleteLater();
    }
}

void CSettingsDialog::initComponents()
{
    setWindowTitle("Settings");
    QString currentPort = CSettingsManager::instance()->paramValue("serial_port");

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->cbSerialPort->addItem(info.portName());

        if (info.portName() == currentPort)
            ui->cbSerialPort->setCurrentText(currentPort);
    }

    mp_serialThread = NULL;
}

void CSettingsDialog::on_pbSerialCheck_clicked()
{
    mp_serialThread = new CSerialThread(ui->cbSerialPort->currentText());
    mp_serialThread->moveToThread(mp_serialThread);
    mp_serialThread->start();

    ui->pbSerialCheck->setEnabled(false);
    ui->cbSerialPort->setEnabled(false);

    connect(mp_serialThread, SIGNAL(openPort(const int&)),
            this, SLOT(at_mp_SerialThread_openPort(const int&)), Qt::UniqueConnection);

    connect(this, SIGNAL(send_getFirmwareID()),
            mp_serialThread, SLOT(on_send_getFirmwareID()), Qt::UniqueConnection);
    connect(mp_serialThread, SIGNAL(received_getFirmwareID(const MeasureUtility::union32_t&)),
            this, SLOT(at_received_getFirmwareID(const MeasureUtility::union32_t&)),
            Qt::UniqueConnection);

    connect(mp_serialThread, SIGNAL(rxTimeout(const int&)),
            this, SLOT(at_mp_SerialThread_rxTimeout(const int&)), Qt::UniqueConnection);

    connect(this, SIGNAL(closePort()),
            mp_serialThread, SLOT(on_closePort()), Qt::UniqueConnection);

    emit send_getFirmwareID();
}

void CSettingsDialog::at_received_getFirmwareID(const MeasureUtility::union32_t& id)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(QString("Connection %1 OK").arg(ui->cbSerialPort->currentText()));
    msgBox.setInformativeText(QString("Embedded system firmware version: %1.%2.%3.%4")
                              .arg(id.id8[3]).arg(id.id8[2]).arg(id.id8[1]).arg(id.id8[0]));
    msgBox.exec();

    ui->pbSerialCheck->setEnabled(true);
    ui->cbSerialPort->setEnabled(true);

    emit closePort();
}

void CSettingsDialog::at_mp_SerialThread_rxTimeout(const int& command)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(QString("Communication with %1 failed").arg(ui->cbSerialPort->currentText()));
    msgBox.setInformativeText(QString("No communication with embedded system! Command %1 without answer")
                              .arg(command));
    msgBox.exec();

    ui->pbSerialCheck->setEnabled(true);
    ui->cbSerialPort->setEnabled(true);

    emit closePort();
}

void CSettingsDialog::at_mp_SerialThread_openPort(const int& val)
{
    if (val)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText(QString("Port %1 is busy").arg(ui->cbSerialPort->currentText()));
        msgBox.setInformativeText(QString("Cannot open specified port."));\
        msgBox.exec();

        ui->pbSerialCheck->setEnabled(true);
        ui->cbSerialPort->setEnabled(true);

        emit closePort();
    }
}

void CSettingsDialog::on_bbButtons_accepted()
{
    QList<SettingParam_t> paramList;

    SettingParam_t serialPort;
    serialPort.m_name = XML_FIELD_PORT;
    serialPort.m_value = ui->cbSerialPort->currentText();
    paramList.append(serialPort);

    CSettingsManager::instance()->writeSettings(paramList);
}











