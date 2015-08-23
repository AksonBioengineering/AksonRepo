#ifndef CGENERICPROJECT_H
#define CGENERICPROJECT_H

#include <QDialog>
#include <QVector>
#include <QIntValidator>

#include "ui_cgenericproject.h"
#include "qcustomplot.h"
#include "MeasureUtility.h"
#include "cserialthread.h"
#include "doublevalidator.h"

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
    void measureStarted();
    void measureFinished();

private slots:
    void mousePress();
    void mouseWheel(QWheelEvent* event);

private:
    virtual void initPlot();
    virtual void initFields();
    virtual void clearData();

protected:
    Ui::CGenericProject *ui;

    QCustomPlot* customPlot;
    QVector<double> m_x;
    QVector<double> m_y;
    QVector<double> m_z;

    CSerialThread* mp_serialThread;

    double m_minFreq = 1;
    double m_maxFreq = 1000000;
};

#endif // CGENERICPROJECT_H
