#ifndef CEISPROJECT_H
#define CEISPROJECT_H

#include "cgenericproject.h"

class CEisProject : public CGenericProject
{
    Q_OBJECT

public:
    CEisProject(QWidget *parent = 0);
    ~CEisProject();

    virtual EMeasures_t measureType(){ return EMeasures_t::eEIS; }
    virtual void takeMeasure();

signals:
    void send_takeMeasEis(const quint8& amplitude, const quint32& freqStart, const quint32& freqEnd,
                          const quint16& nrOfSteps, const MeasureUtility::EStepType_t& stepType);

private:
    virtual void initPlot();
};

#endif // CEISPROJECT_H
