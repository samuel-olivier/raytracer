#ifndef PARALLELOGRAMLIGHT_H
#define PARALLELOGRAMLIGHT_H

#include "AreaLight.h"

class ParallelogramLight : public AreaLight
{
public:
    ParallelogramLight();
    virtual ~ParallelogramLight();

    void    set(QVector3D const& origin, QVector3D const& u, QVector3D const& v);
};

#endif // PARALLELOGRAMLIGHT_H
