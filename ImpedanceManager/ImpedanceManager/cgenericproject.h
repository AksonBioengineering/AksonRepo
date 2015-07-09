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

    EMeasures_t type();

private:
    Ui::CGenericProject *ui;

    virtual void initPlot();

protected:
    QCustomPlot* customPlot;
    QVector<double> m_x;
    QVector<double> m_y;

    EMeasures_t m_type;
};

#endif // CGENERICPROJECT_H
