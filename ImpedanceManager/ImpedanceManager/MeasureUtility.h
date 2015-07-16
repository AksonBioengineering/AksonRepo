#ifndef MEASUREUTILITY_H
#define MEASUREUTILITY_H

#include <QString>

namespace MeasureUtility
{
    enum class EMeasures_t {eDummy = 0, eEIS };

    typedef struct
    {
        QString m_name;
        QString m_value;
    } SettingParam_t;

    typedef union
    {
        quint32 id32;
        quint8 id8[sizeof(quint32)];
    } Uint32Union_t;
}

#endif // MEASUREUTILITY_H
