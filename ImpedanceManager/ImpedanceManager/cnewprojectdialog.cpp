#include "cnewprojectdialog.h"
#include "ui_cnewprojectdialog.h"

CNewProjectDialog::CNewProjectDialog(EMeasures_t* type , QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CNewProjectDialog)
{
    ui->setupUi(this);
    setWindowTitle("Method");

    mp_measure = type;
}

CNewProjectDialog::~CNewProjectDialog()
{
    delete ui;
}



void CNewProjectDialog::on_buttonBox_accepted()
{
    if (ui->rbEis->isChecked())
        *mp_measure = EMeasures_t::eEIS;
}
