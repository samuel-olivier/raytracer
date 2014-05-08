#include "PointLight.h"

PointLight::PointLight()
{
}

PointLight::~PointLight()
{
}

const QVector3D &PointLight::position() const
{
    return _position;
}

void PointLight::setPosition(const QVector3D &position)
{
    _position = position;
}

float PointLight::illuminate(const QVector3D &pos, Color &col, QVector3D &toLight, QVector3D &ltPos)
{
    ltPos = _position;
    toLight = ltPos - pos;
    float bright = intensity() / (toLight.lengthSquared());
    toLight.normalize();
    col = baseColor();
    return bright;
}

int PointLight::sampleNumber() const
{
    return 1;
}
