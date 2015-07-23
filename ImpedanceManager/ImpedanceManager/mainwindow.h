#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QWidget>
#include <QList>
#include <QIcon>

#include <QtGlobal>

#include "ceisproject.h"
#include "csettingsdialog.h"
#include "cnewprojectdialog.h"
#include "csettingsmanager.h"
#include "cserialthread.h"

#define APPNAME  "Impedance Manager "

using namespace MeasureUtility;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void closeSerialThread();
    void send_getFirmwareID();

private slots:
    void at_mp_SerialThread_openPort(const int&);
    void at_mp_SerialThread_rxTimeout(const int&);
    void at_received_getFirmwareID(const MeasureUtility::union32_t&);
    void at_measureStarted();

    void on_action_Settings_triggered();
    void on_action_New_triggered();
    void on_tbMain_tabCloseRequested(int index);
    void on_tbMain_currentChanged(int index);
    void on_tbMain_objectNameChanged(const QString &objectName);
    void on_action_Connect_triggered();

    void on_action_Start_measure_triggered();

private:
    Ui::MainWindow *ui;

    union version_t
    {
        unsigned int ver32;
        unsigned char ver8[sizeof(unsigned int)];
    };
    version_t m_appVersion;
    EMachineState_t m_machineState;

    QString getAppVersion();
    void initComponents();
    CGenericProject* currentMeasObject(const int& index);
    void setMachineState(EMachineState_t state);
    EMachineState_t& machineState();

    CSerialThread* mp_serialThread;
    CGenericProject* mp_dummyProject;
};

#endif // MAINWINDOW_H
