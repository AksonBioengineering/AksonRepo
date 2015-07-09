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

    if (m_serial != NULL)
        delete m_serial;
}

void CSettingsDialog::initComponents()
{
    setWindowTitle("Settings");

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->cbSerialPort->addItem(info.portName());

    m_serial = NULL;
}

void CSettingsDialog::on_pbSerialCheck_clicked()
{
    m_serial =  new QSerialPort(this);

    connect(m_serial, SIGNAL(bytesWritten(qint64)),
                this, SLOT(on_bytesWritten(qint64)));

    m_serial->setPortName(ui->cbSerialPort->currentText());

    if (!m_serial->open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this, tr("Serial port"),
                             tr("Serial port %1 is busy!")
                            .arg(ui->cbSerialPort->currentText()));
    }
    else
    {
        m_serial->setBaudRate(QSerialPort::Baud115200);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);

        if(!m_serial->write("TEST\r\n"))
        {
            QMessageBox::warning(this, tr("Serial port"),
                             tr("SWrite on serial port port %1 failed!")
                            .arg(ui->cbSerialPort->currentText()));
        }
    }
}

void CSettingsDialog::on_bytesWritten(qint64 nrOfBytes)
{
    disconnect(m_serial, SIGNAL(bytesWritten(qint64)),
                    this, SLOT(on_bytesWritten(qint64)));

    qDebug() << "Serial bytes written:" << nrOfBytes;
    m_serial->close();
}



