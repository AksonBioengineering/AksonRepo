#ifndef CGENERICPROJECT_H
#define CGENERICPROJECT_H

#include <QDialog>
#include <QVector>

#include "ui_cgenericproject.h"
#include "qcustomplot.h"
#include "MeasureUtility.h"

using namespace MeasureUtility;

namespace Ui {
class CGenericProject;
}

class CGenericProject : public QDialog
{
    Q_OBJECT

public:
    explicit CGenericProject(QWidget *parent = 0);
    ~CGenericProject();

    virtual EMeasures_t measureType(){ return EMeasures_t::eDummy; }
    virtual void takeMeasure();

private:
    virtual void initPlot();

protected:
    Ui::CGenericProject *ui;

    QCustomPlot* customPlot;
    QVector<double> m_x;
    QVector<double> m_y;
};

#endif // CGENERICPROJECT_H
