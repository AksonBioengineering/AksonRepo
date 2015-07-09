#include "ceisproject.h"

CEisProject::CEisProject()
{
    m_type = EMeasures_t::eEIS;
    initPlot();
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

void CEisProject::exampleEisMethod()
{

}
