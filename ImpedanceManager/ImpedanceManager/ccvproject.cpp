#include "ccvproject.h"

CCvProject::CCvProject(CSerialThread* serialThread, QWidget* parent) : CGenericProject(serialThread, parent)
{
    Q_ASSERT(serialThread);
    //Q_ASSERT(parent);

    initPlot();
    initFields();

    mp_serialThread = serialThread;
    updateTree();
}

CCvProject::~CCvProject()
{

}

void CCvProject::initPlot()
{
    // give the axes some labels:
    customPlot->xAxis->setLabel("U [V]");
    customPlot->yAxis->setLabel("I [A]");

    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
    customPlot->graph(0)->setName("CV measure");

    m_upperXRange = 1.5; // voltage
    m_upperYRange = 0.01; // current

    customPlot->xAxis->setTickStep(0.15);
    customPlot->yAxis->setTickStep(0.001);
    customPlot->xAxis->setRange(0, m_upperXRange);
    customPlot->yAxis->setRange(0, m_upperYRange);

    customPlot->replot();
}

void CCvProject::initFields()
{
    // start potential
    m_labelPotStart.setText("StartPot[mV]");
    ui->glControls->addWidget(&m_labelPotStart, 0, 0);
    m_lePotStart.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_lePotStart, 1, 0);

    // end potential
    m_labelPotEnd.setText("EndPot[mV]");
    ui->glControls->addWidget(&m_labelPotEnd, 0, 1);
    m_lePotEnd.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_lePotEnd, 1, 1);

    // potential step
    m_labelPotStep.setText("PotStep[mV]");
    ui->glControls->addWidget(&m_labelPotStep, 2, 0);
    m_lePotStep.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_lePotStep, 3, 0);

    // number of cycles
    m_labelNrOfCycles.setText("NrOfCycles");
    ui->glControls->addWidget(&m_labelNrOfCycles, 2, 1);
    m_leNrOfCycles.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_leNrOfCycles, 3, 1);

    // Scan delay
    m_labelScanDelay.setText("ScanDelay[ms]");
    ui->glControls->addWidget(&m_labelScanDelay, 4, 0);
    m_leScanDelay.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_leScanDelay, 5, 0);

    QIntValidator* mv16Validator = new QIntValidator(1, 0xFFFF, this);
    QIntValidator* mv8Validator = new QIntValidator(1, 0xFF, this);

    m_lePotStart.setValidator(mv16Validator);
    m_lePotEnd.setValidator(mv16Validator);
    m_leNrOfCycles.setValidator(mv8Validator);
    m_lePotStep.setValidator(mv16Validator);
    m_leScanDelay.setValidator(mv16Validator);
}

void CCvProject::takeMeasure()
{
    bool ok;
    QString badParameters;

    quint16 potStart = (quint16)m_lePotStart.text().toInt(&ok, 10);
    if (!potStart)
        badParameters += m_labelPotStart.text() + "\n";

    quint16 potEnd = (quint16)m_lePotEnd.text().toInt(&ok, 10);
    if (!potEnd)
        badParameters += m_labelPotEnd.text() + "\n";

    quint16 potStep = (quint16)m_lePotStep.text().toInt(&ok, 10);
    if (!potStep)
        badParameters += m_labelPotStep.text() + "\n";

    quint8 nrOfCycles = (quint8)m_leNrOfCycles.text().toInt(&ok, 10);
    if (!nrOfCycles)
        badParameters += m_labelNrOfCycles.text() + "\n";

    quint16 scanDelay = (quint16)m_leScanDelay.text().toInt(&ok, 10);
    if (!scanDelay)
        badParameters += m_labelScanDelay.text() + "\n";

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
    qDebug() << "Sending CV measure request";
    emit send_takeMeasCv(potStart, potEnd, nrOfCycles, potStep, scanDelay);
}

void CCvProject::changeConnections(const bool con)
{
    if(con)
    {
        connect(this, SIGNAL(send_takeMeasCv(const quint16&, const quint16&, const quint8&,
                                             const quint16&, const quint16&)),
        mp_serialThread, SLOT(on_send_takeMeasCv(const quint16&, const quint16&, const quint8&,
                                             const quint16&, const quint16&)), Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_takeMeasCv(const bool&)),
                this, SLOT(on_received_takeMeasCv(const bool&)), Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_giveMeasChunkCv(const quint16&, const union32_t&,
                                                                 const union32_t&)),
                this, SLOT(on_received_giveMeasChunkCv(const quint16&, const union32_t&,
                                                        const union32_t&)), Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_endMeasCv()),
                this, SLOT(on_received_endMeasCv()), Qt::UniqueConnection);
    }
    else
    {
        disconnect(this, SIGNAL(send_takeMeasCv(const quint16&, const quint16&, const quint8&,
                                             const quint16&, const quint16&)),
        mp_serialThread, SLOT(on_send_takeMeasCv(const quint16&, const quint16&, const quint8&,
                                             const quint16&, const quint16&)));

        disconnect(mp_serialThread, SIGNAL(received_takeMeasCv(const bool&)),
                this, SLOT(on_received_takeMeasCv(const bool&)));

        disconnect(mp_serialThread, SIGNAL(received_giveMeasChunkCv(const quint16&, const union32_t&,
                                                                 const union32_t&)),
                this, SLOT(on_received_giveMeasChunkCv(const quint16&, const union32_t&,
                                                        const union32_t&)));

        disconnect(mp_serialThread, SIGNAL(received_endMeasCv()),
                this, SLOT(on_received_endMeasCv()));
    }
}

void CCvProject::on_received_takeMeasCv(const bool& ack)
{
    if (ack)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("Measure init error!");
        msgBox.setInformativeText("CV measure cannot be started");
        msgBox.exec();
    }
    else
        emit measureStarted();
}

void CCvProject::on_received_giveMeasChunkCv(const quint16& sample,
                                             const union32_t& current,
                                             const union32_t& voltage)
{
    m_x.append(current.idFl);
    m_y.append(voltage.idFl);

    addCvPoint(current.idFl, voltage.idFl);
    customPlot->graph(0)->setData(m_x, m_y);
    autoScalePlot();

    qDebug("CV point received. Samp: %u Vol: %f Cur %f", sample, voltage.idFl, current.idFl);
}

void CCvProject::on_received_endMeasCv()
{
    insertLabels();
    emit measureFinished();
}

void CCvProject::updateTree()
{
    QStringList list;
    list << QObject::tr("Voltage[V]") << QObject::tr("Current[A]");

    ui->twPoints->setColumnCount(list.size());
    ui->twPoints->setHeaderLabels(list);
    ui->twPoints->header()->resizeSection(0, 100);
    ui->twPoints->header()->resizeSection(1, 100);
}

int CCvProject::insertLabels()
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
        textLabel->setText(QString("U= %1\nI=%2").arg(m_x[i]).arg(m_y[i]));

        textLabel->setPen(QPen(Qt::black));
        textLabel->setBrush(QBrush(Qt::yellow));

        textLabel->setVisible(m_labelsVisible);
        m_pointLabels.append(textLabel);
    }

    customPlot->replot();
    return 0;
}

void CCvProject::addCvPoint(const float& current, const float& voltage)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->twPoints);

    item->setText(0, QString::number(voltage));
    item->setText(1, QString::number(current));
}

int CCvProject::saveToCsv(QIODevice* device)
{
    Q_ASSERT(device);

    if (m_x.size() != m_y.size())
    {
        qCritical() << "Cannot save to CSV: Different vector x, y sizes!";
        return -1;
    }

    QTextStream outstream(device);
    outstream << "Voltage[V],Current[A]\n";

    for (int i = 0; i < m_x.size(); i++)
    {
        outstream << QString::number(m_x[i]) << ","
                  << QString::number(m_y[i]) << "\n";
    }

    return 0;
}







