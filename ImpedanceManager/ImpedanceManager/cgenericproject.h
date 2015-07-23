#ifndef CGENERICPROJECT_H
#define CGENERICPROJECT_H

#include <QDialog>
#include <QVector>
#include <QIntValidator>

#include "ui_cgenericproject.h"
#include "qcustomplot.h"
#include "MeasureUtility.h"
#include "cserialthread.h"

using namespace MeasureUtility;

namespace Ui {
class CGenericProject;
}

class CGenericProject : public QDialog
{
    Q_OBJECT

public:
    explicit CGenericProject(CSerialThread* serialThread, QWidget *parent = 0);
    ~CGenericProject();

    virtual EMeasures_t measureType(){ return EMeasures_t::eDummy; }
    virtual void takeMeasure();
    virtual void changeConnections(const bool);

signals:
    virtual void measureStarted();
    virtual void measureFinished();

private:
    virtual void initPlot();
    virtual void initFields();

protected:
    Ui::CGenericProject *ui;

    QCustomPlot* customPlot;
    QVector<double> m_x;
    QVector<double> m_y;

    CSerialThread* mp_serialThread;
};

#endif // CGENERICPROJECT_H
