#ifndef CABOUTDIALOG_H
#define CABOUTDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class CAboutDialog;
}

class CAboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CAboutDialog(const QString title, const QString content, QWidget *parent = 0);
    ~CAboutDialog();

private:
    Ui::CAboutDialog *ui;
};

#endif // CABOUTDIALOG_H
