#ifndef CPROJECTMANAGER_H
#define CPROJECTMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>

#include "MeasureUtility.h"

using namespace MeasureUtility;

class CProjectManager : public QObject
{
    Q_OBJECT
public:
    CProjectManager(QFile& file,
                    QList<SettingParam_t>& paramList,
                    const bool writeProj);

signals:

public slots:

};

#endif // CPROJECTMANAGER_H
