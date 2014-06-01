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

void PointLight::sampleRay(Ray &newRay, float &intensity, Color &color) const
{
    do {
        newRay.direction.setX((float(qrand()) / (RAND_MAX * 0.5f)) - 1.0f);
        newRay.direction.setY((float(qrand()) / (RAND_MAX * 0.5f)) - 1.0f);
        newRay.direction.setZ((float(qrand()) / (RAND_MAX * 0.5f)) - 1.0f);
    } while (newRay.direction.lengthSquared() > 1);
    newRay.origin = _position;
    newRay.direction.normalize();
    intensity = this->intensity();
    color = baseColor();
}

int PointLight::sampleNumber() const
{
    return 1;
}
