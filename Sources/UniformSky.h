#ifndef UNIFORMSKY_H
#define UNIFORMSKY_H

#include "Sky.h"

class UniformSky : public Sky
{
public:
    UniformSky(Color const& color = Color::BLACK);
    virtual ~UniformSky();

    Color const&    color() const;
    void            setColor(Color const& color);

    virtual void        evaluate(QVector3D const& direction, Color& color);

private:
    Color   _color;
};

#endif // UNIFORMSKY_H
