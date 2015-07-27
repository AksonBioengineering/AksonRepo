#ifndef CEISPROJECT_H
#define CEISPROJECT_H

#include "cgenericproject.h"

class CEisProject : public CGenericProject
{
    Q_OBJECT

public:
    CEisProject(CSerialThread* serialThread, QWidget *parent = 0);
    ~CEisProject();

    virtual EMeasures_t measureType(){ return EMeasures_t::eEIS; }
    virtual void takeMeasure();
    virtual void changeConnections(const bool);

signals:
    void send_takeMeasEis(const quint8&, const union32_t&, const union32_t&, const quint16&, const quint8&);

private slots:
    void on_received_takeMeasEis(const bool&);
    void on_received_giveMeasChunkEis(const union32_t&, const union32_t&, const union32_t&);
    void on_received_endMeasEis();

private:
    virtual void initPlot();
    virtual void initFields();
};

#endif // CEISPROJECT_H
