#ifndef CNEWPROJECTDIALOG_H
#define CNEWPROJECTDIALOG_H

#include <QDialog>
#include <QDebug>
#include "MeasureUtility.h"

using namespace MeasureUtility;

namespace Ui {
class CNewProjectDialog;
}

class CNewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CNewProjectDialog(EMeasures_t* type , QWidget *parent = 0);
    ~CNewProjectDialog();

private slots:


    void on_buttonBox_accepted();

    void on_rbEis_clicked(bool checked);

    void on_rbCv_clicked(bool checked);

private:
    Ui::CNewProjectDialog *ui;

    EMeasures_t* mp_measure;
};

#endif // CNEWPROJECTDIALOG_H
