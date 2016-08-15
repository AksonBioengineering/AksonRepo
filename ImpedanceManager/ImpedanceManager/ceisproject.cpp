#include "ceisproject.h"

CEisProject::CEisProject(CSerialThread* serialThread, QWidget *parent) :
    CGenericProject(serialThread, parent)
{
    Q_ASSERT(serialThread);
    //Q_ASSERT(parent);

    initPlot();
    initFields();

    mp_serialThread = serialThread;
    updateTree();
}

CEisProject::~CEisProject()
{

}

void CEisProject::initPlot()
{
    // give the axes some labels:
    customPlot->xAxis->setLabel("Re [Ohm]");
    customPlot->yAxis->setLabel("- Im [Ohm]");

    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
    customPlot->graph(0)->setName("EIS measure");

    m_upperXRange = 100000;
    m_lowerXRange = 0;
    m_upperYRange = 100000;
    m_lowerYRange = 0;

    customPlot->xAxis->setTickStep(10000);
    customPlot->yAxis->setTickStep(10000);
    customPlot->xAxis->setRange(m_lowerXRange, m_upperXRange);
    customPlot->yAxis->setRange(m_lowerYRange, m_upperYRange);

    /*
    customPlot->xAxis->setSubTickCount(10);
    customPlot->yAxis->setSubTickCount(10);
    customPlot->xAxis->setScaleLogBase(10);
    customPlot->yAxis->setScaleLogBase(10);
    customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
   */

    customPlot->replot();
}

void CEisProject::initFields()
{
    // amplitude
    m_labelAmplitude.setText("Amp[mV]");
    ui->glControls->addWidget(&m_labelAmplitude, 0, 0);
    m_leAmplitude.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_leAmplitude, 1, 0);

    // freq start
    m_labelFreqStart.setText("Freq start");
    ui->glControls->addWidget(&m_labelFreqStart, 0, 1);
    m_leFreqStart.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_leFreqStart, 1, 1);

    // freq stop
    m_labelFreqStop.setText("Freq stop");
    ui->glControls->addWidget(&m_labelFreqStop, 2, 0);
    m_leFreqStop.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_leFreqStop, 3, 0);

    // freq step
    m_labelFreqStep.setText("Freq step");
    ui->glControls->addWidget(&m_labelFreqStep, 2, 1);
    m_leFreqStep.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_leFreqStep, 3, 1);

    // freq type
    m_labelStepType.setText("Freq type");
    ui->glControls->addWidget(&m_labelStepType, 4, 0);
    m_cbTypeStep.setMaximumWidth(m_maxItemWidth);
    ui->glControls->addWidget(&m_cbTypeStep, 5, 0);

    double m_minFreq = 0.01;
    double m_maxFreq = 1000000;
    DoubleValidator* freQvalidator =
            new DoubleValidator(m_minFreq, m_maxFreq, this);

    m_leFreqStart.setValidator(freQvalidator);
    m_leFreqStep.setValidator(freQvalidator);
    m_leFreqStop.setValidator(freQvalidator);

    QIntValidator* ampValidator = new QIntValidator(1, 200, this);
    m_leAmplitude.setValidator(ampValidator);

    m_cbTypeStep.addItem("Lin");
    m_cbTypeStep.addItem("Log");
}

void CEisProject::takeMeasure()
{
    bool ok;
    QString badParameters;

    quint8 amp = (quint8)m_leAmplitude.text().toInt(&ok, 10);
    if (!amp)
        badParameters += m_labelAmplitude.text() + "\n";

    union32_t freqStart;
    freqStart.idFl = (float)m_leFreqStart.text().toDouble(&ok);
    if (!freqStart.id32)
        badParameters += m_labelFreqStart.text() + "\n";

    union32_t freqEnd;
    freqEnd.idFl = (float)m_leFreqStop.text().toDouble(&ok);
    if (!freqEnd.id32)
        badParameters += m_labelFreqStop.text() + "\n";

    qint16 freqStep = (qint16)m_leFreqStep.text().toInt(&ok, 10);
    if (!freqStep)
        badParameters += m_labelFreqStep.text() + "\n";

    EStepType_t step = (EStepType_t)m_cbTypeStep.currentIndex();

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
    qDebug() << "Sending EIS measure request";
    emit send_takeMeasEis(amp, freqStart, freqEnd, freqStep, (quint8)step);
}

void CEisProject::changeConnections(const bool con)
{
    if(con)
    {
        connect(this, SIGNAL(send_takeMeasEis(const quint8&, const union32_t&, const union32_t&,
                                              const qint16&, const quint8&)),
        mp_serialThread, SLOT(on_send_takeMeasEis(const quint8&, const union32_t&, const union32_t&,
                                              const qint16&, const quint8&)), Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_takeMeasEis(const bool&)),
                this, SLOT(on_received_takeMeasEis(const bool&)), Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_giveMeasChunkEis(const union32_t& , const union32_t& , const union32_t&)),
                this, SLOT(on_received_giveMeasChunkEis(const union32_t& , const union32_t& , const union32_t&)), Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_endMeasEis()),
                this, SLOT(on_received_endMeasEis()), Qt::UniqueConnection);
    }
    else
    {
        disconnect(this, SIGNAL(send_takeMeasEis(const quint8&, const union32_t&, const union32_t&,
                                              const qint16&, const quint8&)),
        mp_serialThread, SLOT(on_send_takeMeasEis(const quint8&, const union32_t&, const union32_t&,
                                              const qint16&, const quint8&)));

        disconnect(mp_serialThread, SIGNAL(received_takeMeasEis(const bool&)),
                this, SLOT(on_received_takeMeasEis(const bool&)));

        disconnect(mp_serialThread, SIGNAL(received_giveMeasChunkEis(const union32_t& , const union32_t& , const union32_t&)),
                this, SLOT(on_received_giveMeasChunkEis(const union32_t& , const union32_t& , const union32_t&)));

        disconnect(mp_serialThread, SIGNAL(received_endMeasEis()),
                this, SLOT(on_received_endMeasEis()));
    }
}

void CEisProject::on_received_takeMeasEis(const bool& ack)
{
    if (ack)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("Measure init error!");
        msgBox.setInformativeText("EIS measure cannot be started");
        msgBox.exec();
    }
    else
        emit measureStarted();
}

void CEisProject::on_received_endMeasEis()
{
    insertLabels();
    emit measureFinished();
}

void CEisProject::on_received_giveMeasChunkEis(const union32_t& realImp,
                                               const union32_t& ImagImp,
                                               const union32_t& freq)
{
    m_x.append(realImp.idFl);
    m_y.append(ImagImp.idFl * -1);
    m_z.append(freq.idFl);

    addEisPoint(realImp.idFl, ImagImp.idFl, freq.idFl);
    //customPlot->graph(0)->setData(m_x, m_y);
    customPlot->graph(0)->addData(realImp.idFl, ImagImp.idFl * -1);
    autoScalePlot();

    qDebug("EIS point received. Real: %f Imag %f Freq %f", realImp.idFl, ImagImp.idFl, freq.idFl);
}

void CEisProject::updateTree()
{
    QStringList list;
    list << QObject::tr("Real[Ohm]") << QObject::tr("Imag[Ohm]") << QObject::tr("Freq[Hz]");

    ui->twPoints->setColumnCount(list.size());
    ui->twPoints->setHeaderLabels(list);
    ui->twPoints->header()->resizeSection(0, 66);
    ui->twPoints->header()->resizeSection(1, 66);
    ui->twPoints->header()->resizeSection(2, 66);
}

void CEisProject::addEisPoint(const float& real, const float& imag, const float& freq)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->twPoints);

    item->setText(0, QString::number(real));
    item->setText(1, QString::number(imag));
    item->setText(2, QString::number(freq));
}

int CEisProject::saveToCsv(QIODevice* device)
{
    Q_ASSERT(device);

    if (m_x.size() != m_y.size() && m_x.size() != m_z.size())
    {
        qCritical() << "Cannot save to CSV: Different vector x, y, z sizes!";
        return -1;
    }

    QTextStream outstream(device);
    outstream << "Real[Ohm],Imaginary[Ohm],Frequency[Hz]\n";

    for (int i = 0; i < m_x.size(); i++)
    {
        outstream << QString::number(m_x[i]) << ","
                  << QString::number(m_y[i]) << ","
                  << QString::number(m_z[i]) << "\n";
    }

    return 0;
}

int CEisProject::insertLabels()
{
    if (m_x.size() != m_y.size() && m_x.size() != m_z.size())
    {
        qCritical() << "Cannot insert labels: Different vector x, y, z sizes!";
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
        textLabel->setText(QString("Real= %1\nImag=%2\nFreq= %3")
                           .arg(m_x[i]).arg(m_y[i] * -1).arg(m_z[i]));

        textLabel->setPen(QPen(Qt::black));
        textLabel->setBrush(QBrush(Qt::yellow));

        textLabel->setVisible(m_labelsVisible);
        m_pointLabels.append(textLabel);
    }

    customPlot->replot();
    return 0;
}








