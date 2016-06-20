#ifndef CCAPROJECT_H
#define CCAPROJECT_H

#include "cgenericproject.h"

class CCaProject : public CGenericProject
{
    Q_OBJECT

public:
    CCaProject(CSerialThread* serialThread, QWidget* parent = 0);
    ~CCaProject();

    virtual EMeasures_t measureType(){ return EMeasures_t::eCA; }
    virtual void takeMeasure();
    virtual void changeConnections(const bool);
    virtual int saveToCsv(QIODevice* device);

signals:
    void send_takeMeasCa(const qint16&, const quint16&, const union32_t&);

private slots:
    void on_received_takeMeasCa(const bool&);
    void on_received_giveMeasChunkCa(const union32_t&, const union32_t&);
    void on_received_endMeasCa();

private:
    virtual void initPlot();
    virtual void initFields();
    virtual void updateTree();

    virtual int insertLabels();

    void addCaPoint(const float& current, const float& time);

    QLabel m_labelPotential;
    QLineEdit m_lePotential;

    QLabel m_labelMeasTime;
    QLineEdit m_leMeasTime;

    QLabel m_label_dt;
    QLineEdit m_le_dt;

public slots:

};

#endif // CCAPROJECT_H
