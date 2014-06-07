#include "LambertMaterial.h"

#include <QDebug>
#include <QtMath>

#include "Config.h"
#include "UniformTexture.hpp"

LambertMaterial::LambertMaterial() : _diffuseColor(new UniformColor(Color::WHITE))
{
    setName("Lambert Material");
}

LambertMaterial::~LambertMaterial()
{
}

Texture* LambertMaterial::diffuseColor() const
{
    return _diffuseColor;
}

void LambertMaterial::setDiffuseColor(Texture *diffuseColor)
{
    _diffuseColor = diffuseColor;
}

void LambertMaterial::setDiffuseColor(const Color &diffuseColor)
{
    UniformColor* t = dynamic_cast<UniformColor*>(_diffuseColor);
    if (t) {
        t->setValue(diffuseColor);
    }
}

void LambertMaterial::computeReflectance(Color &col, const QVector3D &in, const Ray &, const Intersection &hit) const
{
    _diffuseColor->evaluateColor(hit.texCoord, col);
    col.Scale(1.0f / M_PI);
}

bool LambertMaterial::sampleRay(const Ray &, const Intersection &hit, Ray &newRay, Color &intensity) const
{
    float s = float(qrand()) / RAND_MAX;
    float t = float(qrand()) / RAND_MAX;
    float u = 2.0f * M_PI * s;
    float v = qSqrt(1.0f - t);

    newRay.origin = hit.position;
    newRay.direction = hit.normal * qSqrt(t) + hit.u * v * qCos(u) + hit.v * v * qSin(u);
    _diffuseColor->evaluateColor(hit.texCoord, intensity);
    newRay.type = Ray::Diffused;
    return true;
}
