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
}

QString MainWindow::getAppVersion()
{
    return QString("%1.%2.%3").arg(m_appVersion.ver8[2]).arg(m_appVersion.ver8[1]).arg(m_appVersion.ver8[0]) + " ";
}

void MainWindow::initComponents()
{
    qRegisterMetaType< MeasureUtility::Uint32Union_t >("MeasureUtility::Uint32Union_t");

    m_appVersion.ver8[2] = 1;         // Big new functionalities
    m_appVersion.ver8[1] = 0;         // new functionalities
    m_appVersion.ver8[0] = 0;         // changes to existing functionalities
    setWindowTitle(APPNAME + getAppVersion());

    QString settingsFile = QApplication::applicationDirPath() + "/settings.xms";
    CSettingsManager::instance()->setFilePath(settingsFile);
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
    qDebug() << "current:" << ui->tbMain->widget(index)->metaObject()->className();
}

void MainWindow::on_tbMain_objectNameChanged(const QString &objectName)
{
    qDebug() << "Object name changed" << objectName;
}
