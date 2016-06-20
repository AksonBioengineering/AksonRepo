#include "ccaproject.h"

CCaProject::CCaProject(CSerialThread* serialThread, QWidget* parent) :
    CGenericProject(serialThread, parent)
{
    Q_ASSERT(serialThread);

    initPlot();
    initFields();

    mp_serialThread = serialThread;
    updateTree();

   /* // testing
    union32_t x;
    union32_t y;
    x.idFl = 0.5;
    y.idFl = 100;
    on_received_giveMeasChunkCa(y, x);

    x.idFl = 2;
    y.idFl = 300;
    on_received_giveMeasChunkCa(y, x);

    insertLabels();*/
}

CCaProject::~CCaProject()
{

}

void CCaProject::initPlot()
{
    // give the axes some labels:
    customPlot->xAxis->setLabel("t [s]");
    customPlot->yAxis->setLabel("I [uA]");

    customPlot->addGraph();
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
    customPlot->graph(0)->setName("CA measure");

    m_upperXRange = 10; // time, 1h max
    m_lowerXRange = 0;
    m_upperYRange = 1000; // current
    m_lowerYRange = -1000;

    m_tickStepX = 1;
    m_tickStepY = 0.001;

    customPlot->xAxis->setTickStep(m_tickStepX);
    customPlot->yAxis->setTickStep(m_tickStepY);
    customPlot->xAxis->setRange(m_lowerXRange, m_upperXRange);
    customPlot->yAxis->setRange(m_lowerYRange, m_upperYRange);

    customPlot->replot();
}

void CCaProject::initFields()
{
    // potential
    m_labelPotential.setText("Potential[mV]");
    ui->glControls->addWidget(&m_labelPotential, 0, 0);
    m_lePotential.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_lePotential, 1, 0);

    // Measurement time
    m_labelMeasTime.setText("MeasTime[s]");
    ui->glControls->addWidget(&m_labelMeasTime, 2, 0);
    m_leMeasTime.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_leMeasTime, 3, 0);

    // number of cycles
    m_label_dt.setText("dt[s]");
    ui->glControls->addWidget(&m_label_dt, 2, 1);
    m_le_dt.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_le_dt, 3, 1);

    QIntValidator* mv16Validator = new QIntValidator(-1000, 1000, this);
    QIntValidator* time16ValidatorPositive = new QIntValidator(0, 3600, this);
    DoubleValidator* time64dValidatorPositive = new DoubleValidator(0, 10, this);

    m_lePotential.setValidator(mv16Validator);
    m_leMeasTime.setValidator(time16ValidatorPositive);
    m_le_dt.setValidator(time64dValidatorPositive);
}

void CCaProject::on_received_takeMeasCa(const bool& ack)
{
    if (ack)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("Measure init error!");
        msgBox.setInformativeText("CA measure cannot be started");
        msgBox.exec();
    }
    else
        emit measureStarted();
}

void CCaProject::changeConnections(const bool con)
{
    if(con)
    {
        connect(this, SIGNAL(send_takeMeasCa(const qint16&, const quint16&, const union32_t&)),
        mp_serialThread, SLOT(on_send_takeMeasCa(const qint16&, const quint16&, const union32_t&)),
                Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_takeMeasCa(const bool&)),
                this, SLOT(on_received_takeMeasCa(const bool&)), Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_giveMeasChunkCa(const union32_t&, const union32_t&)),
                this, SLOT(on_received_giveMeasChunkCa(const union32_t&, const union32_t&)),
                Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_endMeasCa()),
                this, SLOT(on_received_endMeasCa()), Qt::UniqueConnection);
    }
    else
    {
        disconnect(this, SIGNAL(send_takeMeasCa(const qint16&, const quint16&, const union32_t&)),
        mp_serialThread, SLOT(on_send_takeMeasCa(const qint16&, const quint16&, const union32_t&)));

        disconnect(mp_serialThread, SIGNAL(received_takeMeasCa(const bool&)),
                this, SLOT(on_received_takeMeasCa(const bool&)));

        disconnect(mp_serialThread, SIGNAL(received_giveMeasChunkCa(const union32_t&,
                                                                    const union32_t&)),
                this, SLOT(on_received_giveMeasChunkCa(const union32_t&, const union32_t&)));

        disconnect(mp_serialThread, SIGNAL(received_endMeasCa()),
                this, SLOT(on_received_endMeasCa()));
    }
}

void CCaProject::on_received_giveMeasChunkCa(const union32_t& current, const union32_t& time)
{
    float lcur = current.idFl / 10;

    m_x.append(time.idFl);
    m_y.append(lcur);

    addCaPoint(lcur, time.idFl);
    customPlot->graph(0)->addData(time.idFl, lcur);
    autoScalePlot();

    qDebug("CA point received. Time: %f Cur %f", time.idFl, lcur);
}

void CCaProject::on_received_endMeasCa()
{
    insertLabels();
    emit measureFinished();
}

void CCaProject::updateTree()
{
    QStringList list;
    list << QObject::tr("Time[s]") << QObject::tr("Current[uA]");

    ui->twPoints->setColumnCount(list.size());
    ui->twPoints->setHeaderLabels(list);
    ui->twPoints->header()->resizeSection(0, 100);
    ui->twPoints->header()->resizeSection(1, 100);
}

void CCaProject::takeMeasure()
{
    bool ok;
    QString badParameters;

    qint16 potential = (qint16)m_lePotential.text().toInt(&ok, 10);
    if (!potential)
        badParameters += m_lePotential.text() + "\n";

    quint16 measTime = (quint16)m_leMeasTime.text().toInt(&ok, 10);
    if (!measTime)
        badParameters += m_leMeasTime.text() + "\n";

    union32_t dt;
    dt.idFl = (float)m_le_dt.text().toDouble(&ok);
    if (!dt.id32)
        badParameters += m_le_dt.text() + "\n";

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
    qDebug() << "Sending CA measure request";
    emit send_takeMeasCa(potential, measTime, dt);
}

int CCaProject::saveToCsv(QIODevice* device)
{
    Q_ASSERT(device);

    if (m_x.size() != m_y.size())
    {
        qCritical() << "Cannot save to CSV: Different vector x, y sizes!";
        return -1;
    }

    QTextStream outstream(device);
    outstream << "Time[s],Current[uA]\n";

    for (int i = 0; i < m_x.size(); i++)
    {
        outstream << QString::number(m_x[i]) << ","
                  << QString::number(m_y[i]) << "\n";
    }

    return 0;
}

int CCaProject::insertLabels()
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
        textLabel->setText(QString("t=%1s\nI=%2uA")
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

void CCaProject::addCaPoint(const float& current, const float& time)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->twPoints);

    item->setText(0, QString::number(time, 'e', 2));
    item->setText(1, QString::number(current, 'e', 2));

    ui->twPoints->scrollToBottom();
}







