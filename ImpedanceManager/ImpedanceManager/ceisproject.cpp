#include "ceisproject.h"

CEisProject::CEisProject(CSerialThread* serialThread, QWidget *parent) : CGenericProject(serialThread, parent)
{
    initPlot();
    initFields();

    mp_serialThread = serialThread;
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
    QIntValidator* freQvalidator = new QIntValidator(1, 1000000, this);
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

    emit send_takeMeasEis(amp, freqStart, freqEnd, freqStep, (quint8)step);
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
    emit measureFinished();
}

void CEisProject::on_received_giveMeasChunkEis(const union32_t& realImp,
                                               const union32_t& ImagImp,
                                               const union32_t& freq)
{
    m_x.append(realImp.idFl);
    m_y.append(ImagImp.idFl);
    m_z.append(freq.idFl);

    qDebug("Point received. Real: %f Imag %f Freq %f", m_x.last(), m_y.last(), m_z.last());

    customPlot->graph(0)->setData(m_x, m_y);
    customPlot->replot();
}




