#ifndef DOUBLEVALIDATOR_H
#define DOUBLEVALIDATOR_H

#include <QValidator>

class DoubleValidator : public QValidator
{
    Q_OBJECT
public:
    explicit DoubleValidator(double min, double max, QObject *parent = 0);
    State validate(QString &input, int &pos) const;

signals:

public slots:

private:
    QRegExp mRexp;
    double mMin;
    double mMax;

};

#endif // DOUBLEVALIDATOR_H
