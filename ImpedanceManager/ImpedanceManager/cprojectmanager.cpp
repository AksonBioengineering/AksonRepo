#include "cprojectmanager.h"
#include <QFileInfo>

CProjectManager::CProjectManager(QFile& file,
                                 QList<SettingParam_t>& paramList,
                                 const bool writeProj)
{
    // check either we are saving or reading file
    if (writeProj) // save
    {
        if(!file.exists())
            qDebug() << file.fileName() << "Doesnt exist, need to create new";

        QXmlStreamWriter xw(&file);

        xw.setAutoFormatting(true);
        xw.writeStartDocument();
        xw.writeStartElement("project");

        for (SettingParam_t item : paramList)
            xw.writeTextElement(item.m_name, item.m_value);

        xw.writeEndElement();
        xw.writeEndDocument();
    }
    else // read
    {
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            qWarning() << "Cannot open " + file.fileName();
            return;
        }

        QXmlStreamReader xr(&file);
        SettingParam_t parameter;

        while (!xr.atEnd())
        {
            if (xr.readNextStartElement())
            {
                if (xr.name() != "project")
                {
                    parameter.m_name = xr.name().toString();
                    parameter.m_value = xr.readElementText();

                    if (parameter.m_name != "measType")
                    {
                        paramList.append(parameter);
                        qDebug() << "param found: " + parameter.m_name + " = "
                                    + parameter.m_value;
                    }
                }
            }
        }
    }

    file.close();
}
