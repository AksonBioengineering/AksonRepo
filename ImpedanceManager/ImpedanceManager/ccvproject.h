#ifndef CCVPROJECT_H
#define CCVPROJECT_H

#include "cgenericproject.h"

class CCvProject : public CGenericProject
{
    Q_OBJECT

public:
    CCvProject(CSerialThread* serialThread, QWidget* parent = 0);
    ~CCvProject();

    virtual EMeasures_t measureType(){ return EMeasures_t::eCV; }
    virtual void takeMeasure();
    virtual void changeConnections(const bool);
    virtual int saveToCsv(QIODevice* device);

signals:
    void send_takeMeasCv(const qint16&, const qint16&, const quint8&,
                          const qint16&, const qint16&);

private slots:
    void on_received_takeMeasCv(const bool&);
    void on_received_giveMeasChunkCv(const quint16&, const union32_t&, const union32_t&);
    void on_received_endMeasCv();

private:
    virtual void initPlot();
    virtual void initFields();
    virtual void updateTree();

    virtual int insertLabels();

    void addCvPoint(const float& current, const float& voltage);

    QLabel m_labelPotStart;
    QLineEdit m_lePotStart;

    QLabel m_labelPotEnd;
    QLineEdit m_lePotEnd;

    QLabel m_labelNrOfCycles;
    QLineEdit m_leNrOfCycles;

    QLabel m_labelPotStep;
    QLineEdit m_lePotStep;

    QLabel m_labelScanSpeed;
    QLineEdit m_leScanSpeed;
};

#endif // CCVPROJECT_H
