#ifndef CGENERICPROJECT_H
#define CGENERICPROJECT_H

#include <QDialog>
#include <QVector>
#include <QIntValidator>
#include <QStringList>
#include <QIODevice>

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

    virtual void zoomOut();
    virtual void zoomIn();
    virtual void zoomToPlot();

    virtual int saveToCsv(QIODevice* device);

    void toggleLabels();

signals:
    void measureStarted();
    void measureFinished();

private slots:
    void mousePress();
    void mouseWheel(QWheelEvent* event);

    void rangeYChanged(const QCPRange &newRange, const QCPRange &oldRange);
    void rangeXChanged(const QCPRange &newRange, const QCPRange &oldRange);

private:
    virtual void initPlot();
    virtual void initFields();
    virtual void clearData();
    virtual void updateTree();

    virtual int insertLabels();
    virtual void clearLabels();

protected:
    void autoScalePlot();

    double getYMax();
    double getYMin();
    double getXMax();
    double getXMin();

    void setNewRange(QCPAxis* axis, const double& upperRange,
                                const QCPRange &newRange, const QCPRange &oldRange);
    void setLabelsVisible(bool val);

    Ui::CGenericProject *ui;

    QCustomPlot* customPlot;
    QVector<double> m_x;
    QVector<double> m_y;
    QVector<double> m_z;

    CSerialThread* mp_serialThread;

    double m_minFreq = 1;
    double m_maxFreq = 1000000;
    int m_upperXRange = 1000000;
    int m_upperYRange = 1000000;
    QVector<QCPItemText*> m_pointLabels;
    bool m_labelsVisible;

    constexpr static double zoomInFactor = 1 / 1.5;
    constexpr static double zoomOutFactor = 1.5;

    QStringList m_treeLabels;
};

#endif // CGENERICPROJECT_H
