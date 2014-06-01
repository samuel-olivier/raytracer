#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
}

DirectionalLight::~DirectionalLight()
{
}

const QVector3D &DirectionalLight::direction() const
{
    return _direction;
}

void DirectionalLight::setDirection(const QVector3D &direction)
{
    _direction = direction;
    _direction.normalize();
}

float DirectionalLight::illuminate(const QVector3D &pos, Color &col, QVector3D &toLight, QVector3D &ltPos)
{
    toLight = -_direction;
    col = baseColor();
    ltPos = pos + 1e6 * toLight;
    return intensity();
}

void DirectionalLight::sampleRay(Ray &newRay, float &intensity, Color &color) const
{
    intensity = this->intensity();
    color = baseColor();
}

int DirectionalLight::sampleNumber() const
{
    return 1;
}
