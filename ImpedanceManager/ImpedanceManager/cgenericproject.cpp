#include "cgenericproject.h"

CGenericProject::CGenericProject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CGenericProject)
{
    ui->setupUi(this);
    initPlot();

    m_x.clear();
    m_y.clear();
}

CGenericProject::~CGenericProject()
{
    delete ui;
}

void CGenericProject::initPlot()
{
    customPlot = ui->workPlot;

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes );
    customPlot->setFocusPolicy(Qt::ClickFocus);
    customPlot->xAxis->setAutoTickStep(false);
    customPlot->yAxis->setAutoTickStep(false);
    customPlot->xAxis->setTickStep(100);
    customPlot->yAxis->setTickStep(100);
    customPlot->xAxis->setRange(0, 1000);
    customPlot->yAxis->setRange(0, 1000);
}

void CGenericProject::takeMeasure()
{
    qCritical() << "ERROR: Base class takeMeasure method called!";
}
