#include "doublevalidator.h"

DoubleValidator::DoubleValidator(double min, double max, QObject *parent) :
    QValidator(parent)
    , mRexp("[+]?[0-9]+\\.?[0-9]*([eE][+]?[0-9]+)?")
    , mMin(min)
    , mMax(max)
{
    //Q_ASSERT(parent);
}

DoubleValidator::State DoubleValidator::validate(QString &input, int &pos) const
{
    if (input.isEmpty())
        return Intermediate;

    else if (!mRexp.exactMatch(input))
        return Invalid;

    const double val = input.toDouble();

    if ((mMin <= val) && (val <= mMax))
        return Acceptable;

    else if (val > mMax)
        return Invalid;

    return Intermediate;
}

