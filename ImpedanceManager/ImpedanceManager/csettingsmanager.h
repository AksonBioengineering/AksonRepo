#ifndef CSETTINGSMANAGER_H
#define CSETTINGSMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>

#include "MeasureUtility.h"

#define XML_FIELD_PORT      "serial_port"

using namespace MeasureUtility;

class CSettingsManager : public QObject
{
    Q_OBJECT
private:
    CSettingsManager();
    ~CSettingsManager();

    static CSettingsManager* mp_settingsManager;
    QString m_fileName;

public:
    static CSettingsManager* instance();

    int setFilePath(const QString& path);
    int writeSettings(const QList<SettingParam_t>& paramList);
    int readSettings(QList<SettingParam_t>& paramList);
    QString paramValue(const QString& param);
};

#endif // CSETTINGSMANAGER_H
