#include "caboutdialog.h"
#include "ui_caboutdialog.h"

CAboutDialog::CAboutDialog(const QString title, const QString content, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CAboutDialog)
{
    ui->setupUi(this);

    setWindowTitle(title);
    ui->ptEdit->appendPlainText(content);
    ui->ptEdit->moveCursor(QTextCursor::Start);
}

CAboutDialog::~CAboutDialog()
{
    delete ui;
}
