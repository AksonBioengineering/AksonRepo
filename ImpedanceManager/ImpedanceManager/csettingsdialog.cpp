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
        delete mp_serialThread;
    }
}

void CSettingsDialog::initComponents()
{
    setWindowTitle("Settings");

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->cbSerialPort->addItem(info.portName());

    mp_serialThread = NULL;
}

void CSettingsDialog::on_pbSerialCheck_clicked()
{
    mp_serialThread = new CSerialThread(ui->cbSerialPort->currentText());
    mp_serialThread->moveToThread(mp_serialThread);
    mp_serialThread->start();

    ui->pbSerialCheck->setEnabled(false);

    connect(this, SIGNAL(send_getFirmwareID()),
            mp_serialThread, SLOT(on_send_getFirmwareID()));
    connect(mp_serialThread, SIGNAL(received_getFirmwareID(const MeasureUtility::Uint32Union_t&)),
            this, SLOT(at_received_getFirmwareID(const MeasureUtility::Uint32Union_t&)));

    emit send_getFirmwareID();
}

void CSettingsDialog::at_received_getFirmwareID(const MeasureUtility::Uint32Union_t& id)
{
    qDebug() << QString("Embedded system firmware version: %1.%2.%3.%4")
                .arg(id.id8[3]).arg(id.id8[2]).arg(id.id8[1]).arg(id.id8[0]);
}





