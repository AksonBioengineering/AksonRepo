#ifndef CDPVPROJECT_H
#define CDPVPROJECT_H

#include "cgenericproject.h"

class CDpvProject : public CGenericProject
{
    Q_OBJECT
public:
    CDpvProject(CSerialThread* serialThread, QWidget *parent = 0);
    ~CDpvProject();

    virtual EMeasures_t measureType(){ return EMeasures_t::eDPV; }
    virtual void takeMeasure();
    virtual void changeConnections(const bool);
    virtual int saveToCsv(QIODevice* device);
    virtual int saveProjectAs(QFile& file);
    virtual int openProject(QFile& file);

signals:
    void send_takeMeasDpv(const qint16& qp, const quint16& qt, const quint32& pn,
                          const quint16& pa, const quint16& pp, const quint16& pw,
                          const qint16& ps);
private slots:
    void on_received_takeMeasDpv(const bool&);
    void on_received_giveMeasChunkDpv(const union32_t& current, const union32_t& voltage);
    void on_received_endMeasDpv();

private:
    virtual void initPlot();
    virtual void initFields();
    virtual void updateTree();

    virtual int insertLabels();

    void addDpvPoint(const float& current, const float& voltage);

    QLabel m_labelQp;
    QLineEdit m_leQp;

    QLabel m_labelQt;
    QLineEdit m_leQt;

    QLabel m_labelPn;
    QLineEdit m_lePn;

    QLabel m_labelPa;
    QLineEdit m_lePa;

    QLabel m_labelPp;
    QLineEdit m_lePp;

    QLabel m_labelPw;
    QLineEdit m_lePw;

    QLabel m_labelPs;
    QLineEdit m_lePs;
};

#endif // CDPVPROJECT_H
