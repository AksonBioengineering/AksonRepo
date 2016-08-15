#include "cnewprojectdialog.h"
#include "ui_cnewprojectdialog.h"

CNewProjectDialog::CNewProjectDialog(EMeasures_t* type , QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CNewProjectDialog)
{
    Q_ASSERT(type);
    //Q_ASSERT(parent);

    ui->setupUi(this);
    setWindowTitle("Method");

    mp_measure = type;

    if (ui->rbEis->isChecked())
        ui->lStatus->setText("Electrochemical Impedance Spectroscopy");
}

CNewProjectDialog::~CNewProjectDialog()
{
    delete ui;
}

void CNewProjectDialog::on_buttonBox_accepted()
{
    if (ui->rbEis->isChecked())
        *mp_measure = EMeasures_t::eEIS;
    else if (ui->rbCv->isChecked())
        *mp_measure = EMeasures_t::eCV;
    else if (ui->rbCa->isChecked())
        *mp_measure = EMeasures_t::eCA;
    else if (ui->rbDpv->isChecked())
        *mp_measure = EMeasures_t::eDPV;
}

void CNewProjectDialog::on_rbEis_clicked(bool checked)
{
    if (checked)
        ui->lStatus->setText("Electrochemical Impedance Spectroscopy");
}

void CNewProjectDialog::on_rbCv_clicked(bool checked)
{
    if (checked)
        ui->lStatus->setText("Cyclic Voltametry");
}

void CNewProjectDialog::on_rbSwv_clicked(bool checked)
{
    if (checked)
        ui->lStatus->setText("Square Wave Voltametry");
}

void CNewProjectDialog::on_rbDpv_clicked(bool checked)
{
    if (checked)
        ui->lStatus->setText("Differential Pulse Voltametry");
}

void CNewProjectDialog::on_rbBioFet_clicked(bool checked)
{
    if (checked)
        ui->lStatus->setText("BIOFet transistor");
}

void CNewProjectDialog::on_rbCa_clicked(bool checked)
{
    if (checked)
        ui->lStatus->setText("Chronoamperography");
}
