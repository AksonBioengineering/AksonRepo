#include "cdpvproject.h"

CDpvProject::CDpvProject(CSerialThread* serialThread, QWidget* parent) :
    CGenericProject(serialThread, parent)
{
    Q_ASSERT(serialThread);

    initPlot();
    initFields();

    mp_serialThread = serialThread;
    updateTree();
}

CDpvProject::~CDpvProject()
{

}

void CDpvProject::initPlot()
{
    // give the axes some labels:
    customPlot->xAxis->setLabel("Ewe [mV]");
    customPlot->yAxis->setLabel("di [uA]");

    customPlot->addGraph();
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
    customPlot->graph(0)->setName("DPV measure");

    m_upperXRange = 1500; // mV
    m_lowerXRange = -1500;
    m_upperYRange = 1000; // 1 mA
    m_lowerYRange = -1000;

    m_tickStepX = 1;
    m_tickStepY = 1;

    customPlot->xAxis->setTickStep(m_tickStepX);
    customPlot->yAxis->setTickStep(m_tickStepY);
    customPlot->xAxis->setRange(m_lowerXRange, m_upperXRange);
    customPlot->yAxis->setRange(m_lowerYRange, m_upperYRange);

    customPlot->replot();
}

void CDpvProject::initFields()
{
    // QP: Quiet potential
    m_labelQp.setText("QP[mV]");
    ui->glControls->addWidget(&m_labelQp, 0, 0);
    m_leQp.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_leQp, 1, 0);

    // QT: Quiet time
    m_labelQt.setText("QT[s]");
    ui->glControls->addWidget(&m_labelQt, 0, 1);
    m_leQt.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_leQt, 1, 1);

    // PN: Pulses number
    m_labelPn.setText("PN");
    ui->glControls->addWidget(&m_labelPn, 2, 0);
    m_lePn.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_lePn, 3, 0);

    // PA: Pulse amplitude
    m_labelPa.setText("PA[mV]");
    ui->glControls->addWidget(&m_labelPa, 2, 1);
    m_lePa.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_lePa, 3, 1);

    // PP: Pulse period
    m_labelPp.setText("PP[ms]");
    ui->glControls->addWidget(&m_labelPp, 4, 0);
    m_lePp.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_lePp, 5, 0);

    // PW: Pulse width
    m_labelPw.setText("PW[%]");
    ui->glControls->addWidget(&m_labelPw, 4, 1);
    m_lePw.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_lePw, 5, 1);

    // PS: Potential step
    m_labelPs.setText("PS[mV]");
    ui->glControls->addWidget(&m_labelPs, 6, 0);
    m_lePs.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_lePs, 7, 0);

    QIntValidator* perc16Validator = new QIntValidator(0, 100, this);
    QIntValidator* mv16Validator = new QIntValidator(-1000, 1000, this);
    QIntValidator* mvPos16Validator = new QIntValidator(0, 1000, this);
    QIntValidator* time16Validator = new QIntValidator(0, 10000, this);
    QIntValidator* nr32Validator = new QIntValidator(0, 1000000, this);

    m_leQp.setValidator(mv16Validator);
    m_leQt.setValidator(time16Validator);
    m_lePn.setValidator(nr32Validator);
    m_lePa.setValidator(mvPos16Validator);
    m_lePp.setValidator(time16Validator);
    m_lePw.setValidator(perc16Validator);
    m_lePs.setValidator(mv16Validator);
}

void CDpvProject::updateTree()
{
    QStringList list;
    list << QObject::tr("Voltage[mV]") << QObject::tr("Current[uA]");

    ui->twPoints->setColumnCount(list.size());
    ui->twPoints->setHeaderLabels(list);
    ui->twPoints->header()->resizeSection(0, 100);
    ui->twPoints->header()->resizeSection(1, 100);
}

void CDpvProject::on_received_takeMeasDpv(const bool& ack)
{
    if (ack)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("Measure init error!");
        msgBox.setInformativeText("DPV measure cannot be started");
        msgBox.exec();
    }
    else
        emit measureStarted();
}

void CDpvProject::changeConnections(const bool con)
{
    if(con)
    {
        connect(this, SIGNAL(send_takeMeasDpv(const qint16&, const quint16&, const quint32&,
                                              const quint16&, const quint16&, const quint16&,
                                              const qint16&)),
        mp_serialThread, SLOT(on_send_takeMeasDpv(const qint16&, const quint16&, const quint32&,
                                                  const quint16&, const quint16&, const quint16&,
                                                  const qint16&)), Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_takeMeasDpv(const bool&)),
                this, SLOT(on_received_takeMeasDpv(const bool&)), Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_giveMeasChunkDpv(const union32_t&,
                                                                  const union32_t&)),
                this, SLOT(on_received_giveMeasChunkDpv(const union32_t&, const union32_t&)),
                Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_endMeasDpv()),
                this, SLOT(on_received_endMeasDpv()), Qt::UniqueConnection);
    }
    else
    {
        disconnect(this, SIGNAL(send_takeMeasDpv(const qint16&, const quint16&, const quint32&,
                                              const quint16&, const quint16&, const quint16&,
                                              const qint16&)),
        mp_serialThread, SLOT(on_send_takeMeasDpv(const qint16&, const quint16&, const quint32&,
                                              const quint16&, const quint16&, const quint16&,
                                              const qint16&)));

        disconnect(mp_serialThread, SIGNAL(received_takeMeasDpv(const bool&)),
                this, SLOT(on_received_takeMeasDpv(const bool&)));

        disconnect(mp_serialThread, SIGNAL(received_giveMeasChunkDpv(const union32_t&,
                                                                     const union32_t&)),
                this, SLOT(on_received_giveMeasChunkDpv(const union32_t&, const union32_t&)));

        disconnect(mp_serialThread, SIGNAL(received_endMeasDpv()),
                this, SLOT(on_received_endMeasDpv()));
    }
}

void CDpvProject::on_received_giveMeasChunkDpv(const union32_t& current, const union32_t& voltage)
{
    m_x.append(voltage.idFl);
    m_y.append(current.idFl);

    addDpvPoint(current.idFl, voltage.idFl);
    customPlot->graph(0)->addData(voltage.idFl, current.idFl);
    autoScalePlot();

    qDebug("DPV point received. Vol: %f Cur: %f", voltage.idFl, current.idFl);
}

void CDpvProject::on_received_endMeasDpv()
{
    insertLabels();
    emit measureFinished();
}

void CDpvProject::takeMeasure()
{
    bool ok;
    QString badParameters;

    qint16 qp = (qint16)m_leQp.text().toInt(&ok, 10);
    quint16 qt = (quint16)m_leQt.text().toInt(&ok, 10);
    if (!qt)
        badParameters += m_leQt.text() + "\n";

    quint32 pn = (quint32)m_lePn.text().toInt(&ok, 10);
    if (!pn)
        badParameters += m_lePn.text() + "\n";

    quint16 pa = (quint16)m_lePa.text().toInt(&ok, 10);
    quint16 pp = (quint16)m_lePp.text().toInt(&ok, 10);
    quint16 pw = (quint16)m_lePw.text().toInt(&ok, 10);
    qint16 ps = (qint16)m_lePs.text().toInt(&ok, 10);

    if(badParameters.length())
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("Empty fields detected!");
        msgBox.setInformativeText(badParameters);
        msgBox.exec();
        return;
    }

    clearData();
    qDebug() << "Sending DPV measure request";
    emit send_takeMeasDpv(qp, qt, pn, pa, pp, pw, ps);
}

int CDpvProject::saveToCsv(QIODevice* device)
{
    Q_ASSERT(device);

    if (m_x.size() != m_y.size())
    {
        qCritical() << "Cannot save to CSV: Different vector x, y sizes!";
        return -1;
    }

    QTextStream outstream(device);
    outstream << "Voltage[mV],Current[uA]\n";

    for (int i = 0; i < m_x.size(); i++)
    {
        outstream << QString::number(m_x[i]) << ","
                  << QString::number(m_y[i]) << "\n";
    }

    return 0;
}

int CDpvProject::saveProjectAs(QFile& file)
{
    qDebug() << "Saving DPV project.";

    QList<SettingParam_t> paramList;
    SettingParam_t param;

    param.m_name = "measType";
    param.m_value = QString("%1").arg((int)measureType());
    paramList.append(param);

    param.m_name = "qp";
    param.m_value = m_leQp.text();
    paramList.append(param);

    param.m_name = "qt";
    param.m_value = m_leQt.text();
    paramList.append(param);

    param.m_name = "pn";
    param.m_value = m_lePn.text();
    paramList.append(param);

    param.m_name = "pa";
    param.m_value = m_lePa.text();
    paramList.append(param);

    param.m_name = "pp";
    param.m_value = m_lePp.text();
    paramList.append(param);

    param.m_name = "pw";
    param.m_value = m_lePw.text();
    paramList.append(param);

    param.m_name = "ps";
    param.m_value = m_lePs.text();
    paramList.append(param);

    CProjectManager projMan(file, paramList, true);
    return 0;
}

int CDpvProject::openProject(QFile& file)
{
    qDebug() << "Reading DPV project.";

    QList<SettingParam_t> paramList;
    CProjectManager projMan(file, paramList, false);

    // update fields
    SettingParam_t param;

    param = paramList.takeFirst();
    m_leQp.setText(param.m_value);

    param = paramList.takeFirst();
    m_leQt.setText(param.m_value);

    param = paramList.takeFirst();
    m_lePn.setText(param.m_value);

    param = paramList.takeFirst();
    m_lePa.setText(param.m_value);

    param = paramList.takeFirst();
    m_lePp.setText(param.m_value);

    param = paramList.takeFirst();
    m_lePw.setText(param.m_value);

    param = paramList.takeFirst();
    m_lePs.setText(param.m_value);

    return 0;
}

int CDpvProject::insertLabels()
{
    if (m_x.size() != m_y.size())
    {
        qCritical() << "Cannot insert labels: Different vector x, y sizes!";
        return -1;
    }

    QFont font("Courier", 12);
    for (int i = 0; i < m_x.size(); i++)
    {
        QCPItemText *textLabel = new QCPItemText(customPlot);
        customPlot->addItem(textLabel);
        textLabel->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        textLabel->position->setType(QCPItemPosition::ptPlotCoords);
        textLabel->position->setCoords(m_x[i], m_y[i]); // place position at center/top of axis rect

        textLabel->setFont(font);
        textLabel->setTextAlignment(Qt::AlignLeft);
        textLabel->setText(QString("U=%1mV\nI=%2uA")
                           .arg(QString::number(m_x[i], 'e', 2))
                           .arg(QString::number(m_y[i], 'e', 2)));

        textLabel->setPen(QPen(Qt::black));
        textLabel->setBrush(QBrush(Qt::yellow));

        textLabel->setVisible(m_labelsVisible);
        m_pointLabels.append(textLabel);
    }

    customPlot->replot();
    return 0;
}

void CDpvProject::addDpvPoint(const float& current, const float& voltage)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->twPoints);

    item->setText(0, QString::number(voltage, 'e', 2));
    item->setText(1, QString::number(current, 'e', 2));

    ui->twPoints->scrollToBottom();
}













