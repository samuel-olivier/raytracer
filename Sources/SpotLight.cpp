#include "SpotLight.h"

#include <QtMath>

SpotLight::SpotLight() : _innerAngle(qDegreesToRadians(30.0f))
{
}

SpotLight::~SpotLight()
{
}

float SpotLight::illuminate(const QVector3D &pos, Color &col, QVector3D &toLight, QVector3D &ltPos)
{
    ltPos = _position;
    toLight = ltPos - pos;
    float bright = intensity() / toLight.lengthSquared();
    toLight.normalize();
    float alpha = qAcos(QVector3D::dotProduct(toLight, -_direction));
    if (alpha > _outerAngle * 0.5f) {
        bright = 0.0f;
    } else if (alpha > _innerAngle * 0.5f) {
        bright *= 1.0f - (alpha - _innerAngle * 0.5f) / ((_outerAngle - _innerAngle) * 0.5f);
    }
    col = baseColor();
    return bright;
}

void SpotLight::sampleRay(Ray &newRay, float &intensity, Color &color) const
{
    float angle;
    do {
        newRay.direction.setX((float(qrand()) / (RAND_MAX * 0.5f)) - 1.0f);
        newRay.direction.setY((float(qrand()) / (RAND_MAX * 0.5f)) - 1.0f);
        newRay.direction.setZ((float(qrand()) / (RAND_MAX * 0.5f)) - 1.0f);
        angle = qAcos(QVector3D::dotProduct(_direction, newRay.direction.normalized()));
    } while (newRay.direction.lengthSquared() > 1 || angle > _outerAngle * 0.5f);
    newRay.origin = _position;
    newRay.direction.normalize();
    intensity = this->intensity() * _outerAngle / (M_PI);
    color = baseColor();
}

int SpotLight::sampleNumber() const
{
    return 1;
}

const QVector3D &SpotLight::position() const
{
    return _position;
}

void SpotLight::setPosition(const QVector3D &position)
{
    _position = position;
}

const QVector3D &SpotLight::direction() const
{
    return _direction;
}

void SpotLight::setDirection(const QVector3D &direction)
{
    _direction = direction;
}

float SpotLight::innerAngle() const
{
    return qRadiansToDegrees(_innerAngle);
}

void SpotLight::setInnerAngle(float innerAngle)
{
    _innerAngle = qDegreesToRadians(innerAngle);
}


float SpotLight::outerAngle() const
{
    return qRadiansToDegrees(_outerAngle);
}

void SpotLight::setOuterAngle(float outerAngle)
{
    _outerAngle = qDegreesToRadians(outerAngle);
}
