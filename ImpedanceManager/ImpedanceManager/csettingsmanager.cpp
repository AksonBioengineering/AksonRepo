#include "csettingsmanager.h"

CSettingsManager* CSettingsManager::mp_settingsManager = 0;

CSettingsManager::CSettingsManager() { }

CSettingsManager::~CSettingsManager() { }

CSettingsManager* CSettingsManager::instance()
{
    if (!mp_settingsManager)
        mp_settingsManager = new CSettingsManager();

    return mp_settingsManager;
}

int CSettingsManager::setFilePath(const QString& path)
{
    if (path.isEmpty())
        return -1;

    QFile file(path);
    m_fileName = path;
    return 0;
}

int CSettingsManager::writeSettings(const QList<SettingParam_t>& paramList)
{
    QFile file(m_fileName);

    if(!file.exists())
        qDebug() << m_fileName << "Doesnt exist, need to create new";

    if (!file.open(QFile::WriteOnly | QFile::Text))
        return -3;

    QXmlStreamWriter xw(&file);

    xw.setAutoFormatting(true);
    xw.writeStartDocument();
    xw.writeStartElement("settings");

    for (SettingParam_t item : paramList)
        xw.writeTextElement(item.m_name, item.m_value);

    xw.writeEndElement();
    xw.writeEndDocument();

    file.close();
    return 0;
}

int CSettingsManager::readSettings(QList<SettingParam_t>& paramList)
{
    QFile file(m_fileName);

    if(!file.exists())
        return -1;

    if (m_fileName.isEmpty())
        return -2;

    if (!file.open(QFile::ReadOnly | QFile::Text))
        return -3;

    QXmlStreamReader xr(&file);
    SettingParam_t parameter;

    while (!xr.atEnd())
    {
        if (xr.readNextStartElement())
        {
            if (xr.name() != "settings")
            {
                parameter.m_name = xr.name().toString();
                parameter.m_value = xr.readElementText();
                paramList.append(parameter);
            }
        }
    }

    file.close();
    return 0;
}

QString CSettingsManager::paramValue(const QString& param)
{
    QFile file(m_fileName);

    if(!file.exists())
        return "";

    if (m_fileName.isEmpty())
        return "";

    if (!file.open(QFile::ReadOnly | QFile::Text))
        return "";

    QXmlStreamReader xr(&file);
    QString retVal;
    int itemsFound = 0;

    while (!xr.atEnd())
    {
        if (xr.readNextStartElement())
        {
            if (xr.name() != "settings")
            {
                if (xr.name().toString() == param)
                {
                    itemsFound++;
                    retVal = xr.readElementText();
                }
            }
        }
    }

    file.close();

    if (itemsFound > 1)
        return "";
    else
        return retVal;
}


