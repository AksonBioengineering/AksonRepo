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
}

#endif // MEASUREUTILITY_H
