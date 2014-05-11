#ifndef SKY_H
#define SKY_H

#include <QVector3D>

#include "Color.h"

class Sky
{
public:
    Sky();
    virtual ~Sky();

    virtual void        evaluate(QVector3D const& direction, Color& color);
};

#endif // SKY_H
