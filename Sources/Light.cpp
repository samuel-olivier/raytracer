#include "Light.h"

#include <QtGlobal>
#include <QtMath>

Light::Light()
    : _baseColor(Color::WHITE), _intensity(1.0f)
{
}

Light::~Light()
{
}

const Color &Light::baseColor() const
{
    return _baseColor;
}

void Light::setBaseColor(const Color &baseColor)
{
    _baseColor = baseColor;
}

float Light::intensity() const
{
    return _intensity;
}

void Light::setIntensity(float intensity)
{
    _intensity = qMax(intensity * float(M_PI), 0.0f);
}

void Light::intersectionColor(Color &col)
{
    col = Color::BLACK;
}
