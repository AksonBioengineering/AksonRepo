#include "cgenericproject.h"

CGenericProject::CGenericProject(CSerialThread* serialThread, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CGenericProject)
{
    Q_ASSERT(serialThread);
    //Q_ASSERT(parent);

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
    customCurve = 0;

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes );
    customPlot->setFocusPolicy(Qt::ClickFocus);
    customPlot->setNoAntialiasingOnDrag(true);
    customPlot->xAxis->setAutoTickStep(false);
    customPlot->yAxis->setAutoTickStep(false);

    customPlot->xAxis->setTickLabelRotation(45);
    customPlot->yAxis->setTickLabelRotation(45);

    customPlot->xAxis->setNumberPrecision(2);
    customPlot->yAxis->setNumberPrecision(2);

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
    Q_ASSERT(event);

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
   setNewRange(customPlot->yAxis, m_upperYRange, m_lowerYRange,
               newRange, oldRange, m_tickStepY);
}

void CGenericProject::rangeXChanged(const QCPRange &newRange, const QCPRange &oldRange)
{
   setNewRange(customPlot->xAxis, m_upperXRange, m_lowerXRange,
               newRange, oldRange, m_tickStepX);
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
    m_x.clear();
    m_y.clear();
    m_z.clear();
    ui->twPoints->clear();
    clearLabels();

    m_tickStepXMin = 999999999;
    m_tickStepYMin = 999999999;

    if (customCurve)
        customCurve->clearData();
    else
        customPlot->graph(0)->clearData();

    customPlot->replot();
}

void CGenericProject::updateTree()
{
    qCritical() << "ERROR: Base class updateTree method called!";
}

int CGenericProject::saveToCsv(QIODevice* device)
{
    Q_ASSERT(device);

    qCritical() << "ERROR: Base class saveToCsv method called!";
    return -10;
}

int CGenericProject::saveProjectAs(QFile& file)
{
    qCritical() << "ERROR: Base class saveProjectAs method called for file" + file.fileName();
    return -10;
}

int CGenericProject::openProject(QFile& file)
{
    qCritical() << "ERROR: Base class openProject method called for file" + file.fileName();
    return -10;
}

int CGenericProject::insertLabels()
{
    qCritical() << "ERROR: Base class insertLabels method called!";
    return -10;
}

void CGenericProject::clearLabels()
{
    for (int i = 0; i < m_pointLabels.size(); i++)
        customPlot->removeItem(m_pointLabels[i]);

    m_pointLabels.clear();
}


void CGenericProject::autoScalePlot()
{
    double topX = getXMax();
    double botX = getXMin();
    double topY = getYMax();
    double botY = getYMin();

    if (m_tickStepX < m_tickStepXMin)
        m_tickStepXMin = m_tickStepX;
    if (m_tickStepY < m_tickStepYMin)
        m_tickStepYMin = m_tickStepY;


    if ((topX + m_tickStepXMin) <= m_upperXRange)
        topX += m_tickStepXMin;
    if ((topY + m_tickStepYMin) <= m_upperYRange)
        topY += m_tickStepYMin;

    if ((botX - m_tickStepXMin) >= m_lowerXRange)
        botX -= m_tickStepXMin;
    if ((botY - m_tickStepYMin) >= m_lowerYRange)
        botY -= m_tickStepYMin;

    customPlot->xAxis->setRange(0, 1);
    customPlot->yAxis->setRange(0, 1);
    customPlot->xAxis->setRange((botX + topX) / 2, topX - botX, Qt::AlignCenter);
    customPlot->yAxis->setRange((botY + topY) / 2, topY - botY, Qt::AlignCenter);
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

void CGenericProject::setNewRange(QCPAxis* axis,
                                  const double& upperRange,
                                  const double& lowerRange,
                                  const QCPRange &newRange,
                                  const QCPRange &oldRange,
                                  double& newTickStep)
{
    Q_ASSERT(axis);

    if (newRange.lower < lowerRange)
    {
        if (newRange.upper <= lowerRange)
        {
            axis->setRangeUpper(lowerRange);
        }

        if (axis->scaleType() == QCPAxis::stLinear)
            axis->setRangeLower(0);
        else
            axis->setRangeLower(1);
    }
    else if (newRange.upper <= lowerRange)
    {
        axis->setRangeUpper(lowerRange);
    }

    if (newRange.upper >= upperRange)
    {
        axis->setRangeUpper(upperRange);
    }

    double oldStep = (oldRange.upper - oldRange.lower) / 10;
    newTickStep = (newRange.upper - newRange.lower) / 10;

    if ((newTickStep != oldStep) && (newTickStep > 0))
        axis->setTickStep(newTickStep);
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

void CGenericProject::on_twPoints_itemSelectionChanged()
{
    int index = ui->twPoints->currentIndex().row();

    m_pointLabels[m_lastSelectedItemIndex]->setVisible(false);
    m_pointLabels[index]->setVisible(true);

    m_lastSelectedItemIndex = index;
    customPlot->replot();
}
