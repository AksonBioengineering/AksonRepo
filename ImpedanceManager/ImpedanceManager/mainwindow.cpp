#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initComponents();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete appName;
}

QString MainWindow::getAppVersion()
{
    return QString("%1.%2.%3").arg(appVersion.ver8[2]).arg(appVersion.ver8[1]).arg(appVersion.ver8[0]) + " ";
}

void MainWindow::initComponents()
{
    appVersion.ver8[2] = 1;         // Big new functionalities
    appVersion.ver8[1] = 0;         // new functionalities
    appVersion.ver8[0] = 0;         // changes to existing functionalities
    setWindowTitle(appName + getAppVersion());
}

void MainWindow::on_action_Settings_triggered()
{
    CSettingsDialog* settingsDial = new CSettingsDialog(this);
    settingsDial->show();
}
