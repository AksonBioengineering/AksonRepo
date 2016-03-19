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

    void on_twPoints_itemSelectionChanged();

private:
    virtual void initPlot();
    virtual void initFields();
    virtual void updateTree();

    virtual int insertLabels();

protected:
    void autoScalePlot();
    void clearData();

    double getYMax();
    double getYMin();
    double getXMax();
    double getXMin();

    void setNewRange(QCPAxis* axis, const double& upperRange, const double& lowerRange,
                                const QCPRange &newRange, const QCPRange &oldRange,
                                double& newTickStep);
    void setLabelsVisible(bool val);
    void clearLabels();

    Ui::CGenericProject *ui;

    QCustomPlot* customPlot;
    QCPCurve* customCurve;
    QVector<double> m_x;
    QVector<double> m_y;
    QVector<double> m_z;

    CSerialThread* mp_serialThread;

    double m_upperXRange = 10;
    double m_lowerXRange = 0;
    double m_upperYRange = 10;
    double m_lowerYRange = 0;
    double m_tickStepX = 1;
    double m_tickStepY = 1;
    double m_tickStepXMin = 999999999; // high numbers
    double m_tickStepYMin = 999999999;

    int m_maxItemWidth = 50;
    QVector<QCPItemText*> m_pointLabels;
    bool m_labelsVisible;
    int m_lastSelectedItemIndex = 0;

    constexpr static double zoomInFactor = 1 / 1.5;
    constexpr static double zoomOutFactor = 1.5;

    QStringList m_treeLabels;
};

#endif // CGENERICPROJECT_H
