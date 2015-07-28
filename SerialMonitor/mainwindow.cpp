#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mp_serial = new QSerialPort(this);

    // port
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->cbPort->addItem(info.portName());

    // Baud
    ui->cbBaudRate->addItem("115200");
    ui->cbBaudRate->addItem("57600");
    ui->cbBaudRate->addItem("38400");
    ui->cbBaudRate->addItem("19200");
    ui->cbBaudRate->addItem("9600");

    // Parity
    ui->cbParity->addItem("None");
    ui->cbParity->addItem("Odd");
    ui->cbParity->addItem("Even");

    // Data bits
    ui->cbDataBits->addItem("8");
    ui->cbDataBits->addItem("7");
    ui->cbDataBits->addItem("6");
    ui->cbDataBits->addItem("5");

    // Stop bits
    ui->cbStopBits->addItem("1");
    ui->cbStopBits->addItem("2");

    // Flow controll
    ui->cbFlowCtrl->addItem("None");
    ui->cbFlowCtrl->addItem("Hardware");
    ui->cbFlowCtrl->addItem("Software");
}

MainWindow::~MainWindow()
{
    mp_serial->close();
    delete ui;
}

void MainWindow::on_pbOpen_clicked()
{
    mp_serial->setPortName(ui->cbPort->currentText());

    if (!mp_serial->open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open" << mp_serial->portName();
    }
}
