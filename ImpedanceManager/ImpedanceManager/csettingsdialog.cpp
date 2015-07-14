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
    // tu skonczylem, dodac jeszcze 1 level abstrakcji do seriala

    mp_serialThread = new CSerialThread(ui->cbSerialPort->currentText());
    mp_serialThread->moveToThread(mp_serialThread);
    mp_serialThread->start();

    ui->pbSerialCheck->setEnabled(false);

    connect(this, SIGNAL(sendData(const quint8&, const QByteArray&)),
            mp_serialThread, SLOT(on_sendData(const quint8&, const QByteArray&)));

    quint8 command = 1;
    QByteArray bArray;
    bArray.clear();

    emit sendData(command, bArray);

       /* if(!m_serial->write("TEST\r\n"))
        {
            QMessageBox::warning(this, tr("Serial port"),
                             tr("SWrite on serial port port %1 failed!")
                            .arg(ui->cbSerialPort->currentText()));
        }*/
}





