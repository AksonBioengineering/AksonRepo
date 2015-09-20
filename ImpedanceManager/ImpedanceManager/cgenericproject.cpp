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
    m_labelsVisible = false;
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

    // unables moving range in negative values
    connect(customPlot->yAxis, SIGNAL(rangeChanged(const QCPRange&, const QCPRange&)),
            this, SLOT(rangeYChanged(const QCPRange&, const QCPRange&)));
    connect(customPlot->xAxis, SIGNAL(rangeChanged(const QCPRange&, const QCPRange&)),
            this, SLOT(rangeXChanged(const QCPRange&, const QCPRange&)));
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

void CGenericProject::rangeYChanged(const QCPRange &newRange, const QCPRange &oldRange)
{
   setNewRange(customPlot->yAxis, m_upperYRange, newRange, oldRange);
}

void CGenericProject::rangeXChanged(const QCPRange &newRange, const QCPRange &oldRange)
{
   setNewRange(customPlot->xAxis, m_upperXRange, newRange, oldRange);
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

void CGenericProject::updateTree()
{
    qCritical() << "ERROR: Base class updateTree method called!";
}

int CGenericProject::saveToCsv(QIODevice* device)
{
    qCritical() << "ERROR: Base class saveToCsv method called!";
    return -10;
}

int CGenericProject::insertLabels()
{
    qCritical() << "ERROR: Base class insertLabels method called!";
    return -10;
}

void CGenericProject::clearLabels()
{
    qCritical() << "ERROR: Base class clearLabels method called!";
}

void CGenericProject::autoScalePlot()
{
    double topX = qRound(getXMax());
    double botX = qRound(getXMin());
    double topY = qRound(getYMax());
    double botY = qRound(getYMin());

    if ((topX + 1) <= m_upperXRange)
        topX++;

    if ((topY + 1) <= m_upperYRange)
        topY++;

    if ((botX - 1) >= 0)
        botX--;

    if ((botY - 1) >= 0)
        botY--;

    customPlot->xAxis->setRange(0, 1);
    customPlot->yAxis->setRange(0, 1);
    customPlot->xAxis->setRange(qRound((botX + topX) / 2), topX - botX, Qt::AlignCenter);
    customPlot->yAxis->setRange(qRound((botY + topY) / 2), topY - botY, Qt::AlignCenter);
    customPlot->replot();
}

double CGenericProject::getYMax()
{
    double topVal = 0;

    for (double cell : m_y)
    {
        if (cell > topVal)
            topVal = cell;
    }

    return topVal;
}

double CGenericProject::getYMin()
{
    double botVal = 9999999999; // unreachable number

    for (double cell : m_y)
    {
        if (cell < botVal)
            botVal = cell;
    }

    return botVal;
}

double CGenericProject::getXMax()
{
    double topVal = 0;

    for (double cell : m_x)
    {
        if (cell > topVal)
            topVal = cell;
    }

    return topVal;
}

double CGenericProject::getXMin()
{
    double botVal = 9999999999; // unreachable number

    for (double cell : m_x)
    {
        if (cell < botVal)
            botVal = cell;
    }

    return botVal;
}

void CGenericProject::setNewRange(QCPAxis* axis, const double& upperRange,
                             const QCPRange &newRange, const QCPRange &oldRange)
{
    if (newRange.lower < 1)
    {
        if (newRange.upper <= 1)
        {
            axis->setRangeUpper(1);
        }

        axis->setRangeLower(0);
    }
    else if (newRange.upper <= 1)
    {
        axis->setRangeUpper(1);
    }

    if (newRange.upper >= upperRange)
    {
        axis->setRangeUpper(upperRange);
    }

    double oldStep = qRound((oldRange.upper - oldRange.lower) / 10);
    double step = qRound((newRange.upper - newRange.lower) / 10);

    if ((step != oldStep) && (step > 0))
        axis->setTickStep(step);
}

void CGenericProject::zoomOut()
{
    QCPRange newXRange = customPlot->xAxis->range();
    QCPRange newYRange = customPlot->yAxis->range();

    double centerX = newXRange.lower + ((newXRange.upper - newXRange.lower) / 2);
    double centerY = newYRange.lower + ((newYRange.upper - newYRange.lower) / 2);

    if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        customPlot->xAxis->scaleRange(zoomOutFactor, centerX);
    }
    else if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        customPlot->yAxis->scaleRange(zoomOutFactor, centerY);
    }
    else
    {
        customPlot->xAxis->scaleRange(zoomOutFactor, centerX);
        customPlot->yAxis->scaleRange(zoomOutFactor, centerY);
    }
    customPlot->replot();
}

void CGenericProject::zoomIn()
{
    QCPRange newXRange = customPlot->xAxis->range();
    QCPRange newYRange = customPlot->yAxis->range();

    double centerX = newXRange.lower + ((newXRange.upper - newXRange.lower) / 2);
    double centerY = newYRange.lower + ((newYRange.upper - newYRange.lower) / 2);

    if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
       if ((newXRange.upper - newXRange.lower) > 1)
            customPlot->xAxis->scaleRange(zoomInFactor, centerX);
    }
    else if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        if ((newYRange.upper - newYRange.lower) > 1)
            customPlot->yAxis->scaleRange(zoomInFactor, centerY);
    }
    else
    {   if ((newXRange.upper - newXRange.lower) > 1)
            customPlot->xAxis->scaleRange(zoomInFactor, centerX);
        if ((newYRange.upper - newYRange.lower) > 1)
            customPlot->yAxis->scaleRange(zoomInFactor, centerY);
    }
    customPlot->replot();
}

void CGenericProject::zoomToPlot()
{
    autoScalePlot();
}

void CGenericProject::setLabelsVisible(bool val)
{
    m_labelsVisible = val;

    foreach (QCPItemText* item, m_pointLabels)
    {
        item->setVisible(val);
    }

    customPlot->replot();
}

void CGenericProject::toggleLabels()
{
    m_labelsVisible ^= 1;
    setLabelsVisible(m_labelsVisible);
}




