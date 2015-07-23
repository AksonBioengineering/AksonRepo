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

    quint32 freqStart = (quint32)ui->leFreqStart->text().toInt(&ok, 10);
    if (!freqStart)
        badParameters += ui->lFreqStart->text() + "\n";

    quint32 freqEnd = (quint32)ui->leFreqStop->text().toInt(&ok, 10);
    if (!freqEnd)
        badParameters += ui->lFreqEnd->text() + "\n";

    quint32 freqStep = (quint32)ui->leFreqStep->text().toInt(&ok, 10);
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
        connect(this, SIGNAL(send_takeMeasEis(const quint8&, const quint32&, const quint32&,
                                              const quint16&, const quint8&)),
        mp_serialThread, SLOT(on_send_takeMeasEis(const quint8&, const quint32&, const quint32&,
                                              const quint16&, const quint8&)), Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_takeMeasEis(const bool&)),
                this, SLOT(on_received_takeMeasEis(const bool&)), Qt::UniqueConnection);

        connect(mp_serialThread, SIGNAL(received_giveMeasChunkEis(const union32_t& , const union32_t& , const union32_t&)),
                this, SLOT(on_received_giveMeasChunkEis(const union32_t& , const union32_t& , const union32_t&)), Qt::UniqueConnection);
    }
    else
    {
        disconnect(this, SIGNAL(send_takeMeasEis(const quint8&, const quint32&, const quint32&,
                                              const quint16&, const quint8&)),
        mp_serialThread, SLOT(on_send_takeMeasEis(const quint8&, const quint32&, const quint32&,
                                              const quint16&, const quint8&)));

        disconnect(mp_serialThread, SIGNAL(received_takeMeasEis(const bool&)),
                this, SLOT(on_received_takeMeasEis(const bool&)));

        disconnect(mp_serialThread, SIGNAL(received_giveMeasChunkEis(const union32_t& , const union32_t& , const union32_t&)),
                this, SLOT(on_received_giveMeasChunkEis(const union32_t& , const union32_t& , const union32_t&)));
    }
}

void CEisProject::on_received_takeMeasEis(const bool& ack)
{
    if (!ack)
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

void CEisProject::on_received_giveMeasChunkEis(const union32_t& realImp, const union32_t& ImagImp, const union32_t& freq)
{

}




