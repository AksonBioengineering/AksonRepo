#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QWidget>
#include <QList>

#include "ceisproject.h"
#include "csettingsdialog.h"
#include "cnewprojectdialog.h"
#include "csettingsmanager.h"

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

private slots:
    void on_action_Settings_triggered();
    void on_action_New_triggered();
    void on_tbMain_tabCloseRequested(int index);


    void on_tbMain_currentChanged(int index);

    void on_tbMain_objectNameChanged(const QString &objectName);

    void on_action_Connect_triggered();

private:
    Ui::MainWindow *ui;

    union version_t
    {
        unsigned int ver32;
        unsigned char ver8[sizeof(unsigned int)];
    };
    version_t m_appVersion;

    QString getAppVersion();
    void initComponents();
    CGenericProject* currentMeasObject();

};

#endif // MAINWINDOW_H
