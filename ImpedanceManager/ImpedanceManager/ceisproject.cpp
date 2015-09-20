#include "ceisproject.h"

CEisProject::CEisProject(CSerialThread* serialThread, QWidget *parent) : CGenericProject(serialThread, parent)
{
    initPlot();
    initFields();

    mp_serialThread = serialThread;

    m_minFreq = 0.01;
    m_maxFreq = 1000000;

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

    customPlot->replot();
}

void CEisProject::initFields()
{
    DoubleValidator* freQvalidator =
            new DoubleValidator(m_minFreq, m_maxFreq, this);

    ui->leFreqStart->setValidator(freQvalidator);
    ui->leFreqStep->setValidator(freQvalidator);
    ui->leFreqStop->setValidator(freQvalidator);

    QIntValidator* ampValidator = new QIntValidator(1, 200, this);
    ui->leAmplitude->setValidator(ampValidator);

    ui->cbTypeStep->addItem("Lin");
    ui->cbTypeStep->addItem("Log");
}

void CEisProject::takeMeasure()
{
    qDebug() << "Sending EIS measure request";

    bool ok;
    QString badParameters;

    quint8 amp = (quint8)ui->leAmplitude->text().toInt(&ok, 10);
    if (!amp)
        badParameters += ui->lAmplitude->text() + "\n";

    union32_t freqStart;
    freqStart.idFl = (float)ui->leFreqStart->text().toDouble(&ok);
    if (!freqStart.id32)
        badParameters += ui->lFreqStart->text() + "\n";

    union32_t freqEnd;
    freqEnd.idFl = (float)ui->leFreqStop->text().toDouble(&ok);
    if (!freqEnd.id32)
        badParameters += ui->lFreqEnd->text() + "\n";

    quint16 freqStep = (quint16)ui->leFreqStep->text().toInt(&ok, 10);
    if (!freqStep)
        badParameters += ui->lFreqStep->text() + "\n";

    EStepType_t step = (EStepType_t)ui->cbTypeStep->currentIndex();

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

    emit send_takeMeasEis(amp, freqStart, freqEnd, freqStep, (quint8)step);
}

void CEisProject::clearData()
{
    m_x.clear();
    m_y.clear();
    m_z.clear();
    ui->twPoints->clear();
    clearLabels();

    customPlot->graph(0)->clearData();
    customPlot->replot();
}

void CEisProject::changeConnections(const bool con)
{
    if(con)
    {
        connect(this, SIGNAL(send_takeMeasEis(const quint8&, const union32_t&, const union32_t&,
                                              const quint16&, const quint8&)),
        mp_serialThread, SLOT(on_send_takeMeasEis(const quint8&, const union32_t&, const union32_t&,
                                              const quint16&, const quint8&)), Qt::UniqueConnection);

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
                                              const quint16&, const quint8&)),
        mp_serialThread, SLOT(on_send_takeMeasEis(const quint8&, const union32_t&, const union32_t&,
                                              const quint16&, const quint8&)));

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
    {
        emit measureStarted();
    }
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
    customPlot->graph(0)->setData(m_x, m_y);
    autoScalePlot();

    qDebug("Point received. Real: %f Imag %f Freq %f", realImp.idFl, ImagImp.idFl, freq.idFl);
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
        qCritical() << "Cannot save to CSV: Different vector x, y, z sizes!";
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

void CEisProject::clearLabels()
{
    for (int i = 0; i < m_pointLabels.size(); i++)
    {
        customPlot->removeItem(m_pointLabels[i]);
    }

    m_pointLabels.clear();
}






