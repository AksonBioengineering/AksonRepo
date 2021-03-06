#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QWidget>
#include <QList>
#include <QIcon>

#include <QtGlobal>

#include "ceisproject.h"
#include "ccvproject.h"
#include "ccaproject.h"
#include "cdpvproject.h"

#include "csettingsdialog.h"
#include "cnewprojectdialog.h"
#include "csettingsmanager.h"
#include "cserialthread.h"
#include "caboutdialog.h"

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
    MainWindow(const QString& fileToOpen, QWidget *parent = 0);
    ~MainWindow();

signals:
    void closeSerialThread();
    void send_getFirmwareID();

private slots:
    void at_mp_SerialThread_openPort(const int&);
    void at_mp_SerialThread_rxTimeout(const int&);
    void at_received_getFirmwareID(const MeasureUtility::union32_t&);
    void at_measureStarted();
    void at_measureFinished();

    void on_action_Settings_triggered();
    void on_action_New_triggered();
    void on_tbMain_tabCloseRequested(int index);
    void on_tbMain_currentChanged(int index);
    void on_tbMain_objectNameChanged(const QString &objectName);
    void on_action_Connect_triggered();

    void on_action_Start_measure_triggered();

    void on_action_Zoom_out_triggered();

    void on_action_Zoom_in_triggered();

    void on_action_Zoom_to_screen_triggered();

    void on_action_Export_CSV_triggered();

    void on_action_Points_labels_triggered();

    void on_action_About_triggered();

    void on_action_Save_triggered();

    void on_action_Save_as_triggered();

    void on_action_Open_triggered();

private:
    void saveCsvFile(const QString& fileName);
    const QString getNameForSave();
    const QString getNameForOpen();
    int saveProject(const QString& fileName);
    void openProject(const QString& fileName);
    QString getAppVersion();
    void initComponents();
    CGenericProject* currentMeasObject(const int& index);
    void setMachineState(EMachineState_t state);
    EMachineState_t& machineState();
    void checkCurrentTab(int index);

    Ui::MainWindow *ui;

    union version_t
    {
        unsigned int ver32;
        unsigned char ver8[sizeof(unsigned int)];
    };
    version_t m_appVersion;
    EMachineState_t m_machineState;

    CSerialThread* mp_serialThread;
    CGenericProject* mp_dummyProject;
};

#endif // MAINWINDOW_H
