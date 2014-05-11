#include "UniformSky.h"

UniformSky::UniformSky(const Color &color)
{
    setColor(color);
}

UniformSky::~UniformSky()
{
}

const Color &UniformSky::color() const
{
    return _color;
}

void UniformSky::setColor(const Color &color)
{
    _color = color;
}

void UniformSky::evaluate(const QVector3D &, Color &color)
{
    color = _color;
}
