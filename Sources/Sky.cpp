#include "Sky.h"

#include "UniformTexture.hpp"

Sky::Sky()
{
}

Sky::~Sky()
{
}

void Sky::evaluate(const QVector3D &, Color &color)
{
    color = Color::BLACK;
}
