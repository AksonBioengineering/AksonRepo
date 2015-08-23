#include "cgenericproject.h"

CGenericProject::CGenericProject(CSerialThread* serialThread, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CGenericProject)
{
    ui->setupUi(this);
    initPlot();

    m_x.clear();
    m_y.clear();
    m_z.clear();

    mp_serialThread = serialThread;
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
    customPlot->xAxis->setTickStep(10000);
    customPlot->yAxis->setTickStep(10000);
    customPlot->xAxis->setRange(0, 100000);
    customPlot->yAxis->setRange(0, 100000);

    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(customPlot, SIGNAL(mousePress(QMouseEvent*)),
            this, SLOT(mousePress()));
    connect(customPlot, SIGNAL(mouseWheel(QWheelEvent*)),
            this, SLOT(mouseWheel(QWheelEvent*)));
}

void CGenericProject::mousePress()
{
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        customPlot->axisRect()->setRangeDrag(customPlot->xAxis->orientation());
    else if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        customPlot->axisRect()->setRangeDrag(customPlot->yAxis->orientation());
    else
        customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void CGenericProject::mouseWheel(QWheelEvent* event)
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed
    if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        customPlot->axisRect()->setRangeZoom(customPlot->xAxis->orientation());
    else if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        customPlot->axisRect()->setRangeZoom(customPlot->yAxis->orientation());
    else
        customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void CGenericProject::takeMeasure()
{
    qCritical() << "ERROR: Base class takeMeasure method called!";
}

void CGenericProject::initFields()
{
    qCritical() << "ERROR: Base class initFields method called!";
}

void CGenericProject::changeConnections(const bool)
{
    qCritical() << "ERROR: Base class changeConnections method called!";
}

void CGenericProject::clearData()
{
    qCritical() << "ERROR: Base class clearData method called!";
}
