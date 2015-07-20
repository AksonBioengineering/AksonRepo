#ifndef MEASUREUTILITY_H
#define MEASUREUTILITY_H

#include <QString>

namespace MeasureUtility
{
    enum class EMeasures_t { eDummy = 0, eEIS };
    enum class EStepType_t { eLinear = 0, eLog };

    typedef struct
    {
        QString m_name;
        QString m_value;
    } SettingParam_t;

    typedef union
    {
        quint32 id32;
        float idFl;
        quint8 id8[sizeof(quint32)];
    } union32_t;
}

#endif // MEASUREUTILITY_H
