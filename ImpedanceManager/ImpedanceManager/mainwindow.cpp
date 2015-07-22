#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initComponents();

    // connections:
    connect(this, SIGNAL(closeSerialThread()),
            mp_serialThread, SLOT(on_closePort()));

    connect(mp_serialThread, SIGNAL(openPort(const int&)),
            this, SLOT(at_mp_SerialThread_openPort(const int&)), Qt::UniqueConnection);

    connect(this, SIGNAL(send_getFirmwareID()),
            mp_serialThread, SLOT(on_send_getFirmwareID()), Qt::UniqueConnection);
    connect(mp_serialThread, SIGNAL(received_getFirmwareID(const MeasureUtility::union32_t&)),
            this, SLOT(at_received_getFirmwareID(const MeasureUtility::union32_t&)),
            Qt::UniqueConnection);

    connect(mp_serialThread, SIGNAL(rxTimeout(const int&)),
            this, SLOT(at_mp_SerialThread_rxTimeout(const int&)), Qt::UniqueConnection);
}

MainWindow::~MainWindow()
{
    delete ui;

    if (mp_serialThread)
    {
        mp_serialThread->quit();
        mp_serialThread->wait();
        mp_serialThread->deleteLater();
    }
}

QString MainWindow::getAppVersion()
{
    return QString("%1.%2.%3").arg(m_appVersion.ver8[2]).arg(m_appVersion.ver8[1]).arg(m_appVersion.ver8[0]) + " ";
}

void MainWindow::initComponents()
{
    qRegisterMetaType< MeasureUtility::union32_t >("MeasureUtility::union32_t");

    m_appVersion.ver8[2] = 1;         // Big new functionalities
    m_appVersion.ver8[1] = 0;         // new functionalities
    m_appVersion.ver8[0] = 0;         // changes to existing functionalities
    setWindowTitle(APPNAME + getAppVersion());

    QString settingsFile = QApplication::applicationDirPath() + "/settings.xms";
    CSettingsManager::instance()->setFilePath(settingsFile);
    setMachineState(EMachineState_t::eDisconnected);

    mp_serialThread = new CSerialThread(CSettingsManager::instance()->paramValue(XML_FIELD_PORT));
    mp_serialThread->moveToThread(mp_serialThread);

    mp_dummyProject = NULL;
}

CGenericProject* MainWindow::currentMeasObject()
{
    auto measObj = dynamic_cast<CGenericProject*>(ui->tbMain->widget(ui->tbMain->currentIndex()));

    if (!measObj)
    {
        if (!mp_dummyProject)
            mp_dummyProject = new CGenericProject(this);

        measObj = mp_dummyProject; // tu skonczylem
    }

    return measObj;
}

void MainWindow::setMachineState(EMachineState_t state)
{
    static EMachineState_t oldState = EMachineState_t::eDisconnected;
    m_machineState = state;

    static QIcon iconConnected;
    iconConnected.addFile(QStringLiteral(":/24/sign-ban-lighting.png"), QSize(), QIcon::Normal, QIcon::Off);

    static QIcon iconDisconnected;
    iconDisconnected.addFile(QStringLiteral(":/24/lightning.png"), QSize(), QIcon::Normal, QIcon::Off);

    QString port = CSettingsManager::instance()->paramValue(XML_FIELD_PORT);

    switch (state)
    {
        case EMachineState_t::eDisconnected:
        {
            ui->action_Connect->setEnabled(true);
            ui->action_Connect->setIcon(iconDisconnected);
            ui->action_Connect->setToolTip(QString("Connect to %1").arg(port));
            ui->action_Start_measure->setEnabled(false);
            ui->action_Pause_measure->setEnabled(false);
            ui->action_Cancel_measure->setEnabled(false);
            ui->statusBar->showMessage(QString("Disconnected from %1").arg(port), 5000);
            break;
        }

        case EMachineState_t::eConnected:
        {
            ui->action_Connect->setEnabled(true);
            ui->action_Connect->setIcon(iconConnected);
            ui->action_Connect->setToolTip(QString("Disconnect from %1").arg(port));
            ui->action_Start_measure->setEnabled(true);
            ui->action_Pause_measure->setEnabled(false);
            ui->action_Cancel_measure->setEnabled(false);
            break;
        }

        case EMachineState_t::eConnecting:
        {
            ui->action_Connect->setEnabled(false);
            ui->action_Connect->setIcon(iconDisconnected);
            ui->action_Connect->setToolTip(QString("Connecting to %1...").arg(port));
            ui->action_Start_measure->setEnabled(false);
            ui->action_Pause_measure->setEnabled(false);
            ui->action_Cancel_measure->setEnabled(false);
            ui->statusBar->showMessage(QString("Connecting to %1...").arg(port), 5000);
            break;
        }

        case EMachineState_t::eMeasuring:
        {
            ui->action_Connect->setEnabled(true);
            ui->action_Connect->setIcon(iconConnected);
            ui->action_Connect->setToolTip(QString("Disconnect from %1").arg(port));
            ui->action_Start_measure->setEnabled(false);
            ui->action_Pause_measure->setEnabled(true);
            ui->action_Cancel_measure->setEnabled(true);
            break;
        }

        default:
        {
            qWarning() << "Unknown machine state with code" << (int)state;
            m_machineState = oldState;
        }
    }

    if (m_machineState != oldState)
        oldState = m_machineState;
}

EMachineState_t& MainWindow::machineState()
{
    return m_machineState;
}

void MainWindow::on_action_Settings_triggered()
{
    CSettingsDialog settingsDial;
    settingsDial.exec();
}

void MainWindow::on_action_New_triggered()
{
    EMeasures_t* newMeasure = new EMeasures_t;
    CNewProjectDialog newDial(newMeasure, this);

    if (newDial.exec())
    {
        qDebug() << "selected measure:" << (int)*newMeasure;

        switch (*newMeasure)
        {
            case EMeasures_t::eEIS:
            {
                CGenericProject* test = new CEisProject();
                ui->tbMain->addTab(test, "Untitled*");
                break;
            }

            default:
            {
                qWarning() << "Choosen unknown measure method, forgot to add?";
            }
        }
    }

    delete newMeasure;
}

void MainWindow::on_tbMain_tabCloseRequested(int index)
{
    qDebug() << "Close request " << index;
    delete  ui->tbMain->widget(index);
    //ui->tbMain->removeTab(index);
}

void MainWindow::on_tbMain_currentChanged(int index)
{
    //qDebug() << "current:" << ui->tbMain->widget(index)->metaObject()->className();
}

void MainWindow::on_tbMain_objectNameChanged(const QString &objectName)
{
    qDebug() << "Object name changed" << objectName;
}

void MainWindow::on_action_Connect_triggered()
{
    qDebug() << "Current object is of type" << (int)currentMeasObject()->measureType();

    if (EMachineState_t::eDisconnected == machineState())
    {
        mp_serialThread->start();
    }
    else
    {
        emit closeSerialThread();
        setMachineState(EMachineState_t::eDisconnected);

    }
}

void MainWindow::at_received_getFirmwareID(const MeasureUtility::union32_t& id)
{
    setMachineState(EMachineState_t::eConnected);
    ui->statusBar->showMessage(QString("Embedded system firmware version: %1.%2.%3.%4")
                              .arg(id.id8[3]).arg(id.id8[2]).arg(id.id8[1]).arg(id.id8[0]), 5000);
}

void MainWindow::at_mp_SerialThread_rxTimeout(const int& command)
{
    setMachineState(EMachineState_t::eDisconnected);

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(QString("Communication with %1 failed")
                   .arg(CSettingsManager::instance()->paramValue(XML_FIELD_PORT)));
    msgBox.setInformativeText(QString("No communication with embedded system! Command %1 without answer")
                              .arg(command));
    msgBox.exec();
}

void MainWindow::at_mp_SerialThread_openPort(const int& val)
{
    if (val)
    {
        setMachineState(EMachineState_t::eDisconnected);

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText(QString("Port %1 is bussy").arg(CSettingsManager::instance()->paramValue(XML_FIELD_PORT)));
        msgBox.setInformativeText(QString("Cannot open specified port."));\
        msgBox.exec();
    }
    else
    {
        setMachineState(EMachineState_t::eConnecting);
        emit send_getFirmwareID();
    }
}
