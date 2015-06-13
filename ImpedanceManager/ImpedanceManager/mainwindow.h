#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "csettingsdialog.h"

namespace Ui {
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

private:
    Ui::MainWindow *ui;

    union version_t
    {
        unsigned int ver32;
        unsigned char ver8[sizeof(unsigned int)];
    };
    version_t appVersion;
    constexpr static char* appName = "Impedance Manager ";

    QString getAppVersion();
    void initComponents();
};

#endif // MAINWINDOW_H
