#include "ceisproject.h"

CEisProject::CEisProject(QWidget *parent) : CGenericProject(parent)
{
    initPlot();
    initFields();
}

CEisProject::~CEisProject()
{

}

void CEisProject::initPlot()
{
    // give the axes some labels:
    customPlot->xAxis->setLabel("Re [Ohm]");
    customPlot->yAxis->setLabel("- Im [Ohm]");

    customPlot->replot();
}

void CEisProject::initFields()
{
    QIntValidator* freQvalidator = new QIntValidator(1, 1000000, this);
    ui->leFreqStart->setValidator(freQvalidator);
    ui->leFreqStep->setValidator(freQvalidator);
    ui->leFreqStop->setValidator(freQvalidator);

    QIntValidator* ampValidator = new QIntValidator(1, 200, this);
    ui->leAmplitude->setValidator(ampValidator);

    ui->cbTypeStep->addItem("Lin");
    ui->cbTypeStep->addItem("Log");
}

void CEisProject::takeMeasure()
{
    bool ok;
    emit send_takeMeasEis((quint8)ui->lAmplitude->text().toInt(&ok, 10),
                          (quint32)ui->leFreqStart->text().toInt(&ok, 10),
                          (quint32)ui->leFreqStop->text().toInt(&ok, 10),
                          (quint32)ui->leFreqStep->text().toInt(&ok, 10),
                          (EStepType_t)ui->cbTypeStep->currentIndex());
}




