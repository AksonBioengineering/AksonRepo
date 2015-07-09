#ifndef CEISPROJECT_H
#define CEISPROJECT_H

#include "cgenericproject.h"

class CEisProject : public CGenericProject
{
public:
    CEisProject();
    ~CEisProject();

private:
    virtual void initPlot();
    void exampleEisMethod();
};

#endif // CEISPROJECT_H
