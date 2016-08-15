#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString fileToOpen = NULL;

    if (argc >= 2)
    {
        fileToOpen = argv[1];
        QFile file(fileToOpen);

        if(!file.exists())
        {
            qWarning() << "File " + fileToOpen + " doesnt exist";
            fileToOpen = "";
        }

        if (fileToOpen.isEmpty())
        {
            qWarning() << "File " + fileToOpen + " is empty";
            fileToOpen = "";
        }
    }

    qDebug() << "Program started with file to open: " + fileToOpen;

    MainWindow w(fileToOpen, 0);
    w.show();

    return a.exec();
}
