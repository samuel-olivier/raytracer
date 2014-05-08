#include "LambertMaterial.h"

#include <QDebug>
#include <QtMath>

#include "Config.h"

LambertMaterial::LambertMaterial() : _diffuseColor(Color::WHITE)
{
    setName("Lambert Material");
}

LambertMaterial::~LambertMaterial()
{
}

const Color &LambertMaterial::diffuseColor() const
{
    return _diffuseColor;
}

void LambertMaterial::setDiffuseColor(const Color &diffuseColor)
{
    _diffuseColor = diffuseColor;
}

void LambertMaterial::computeReflectance(Color &col, const QVector3D &in, const Ray &, const Intersection &hit) const
{
    col = _diffuseColor;
    col.Scale(QVector3D::dotProduct(in, hit.normal));
}

void LambertMaterial::sampleRay(const Ray &, const Intersection &hit, Ray &newRay, Color &intensity) const
{
    float s = float(qrand()) / RAND_MAX;
    float t = float(qrand()) / RAND_MAX;
    float u = 2.0f * M_PI * s;
    float v = qSqrt(1.0f - t);

    newRay.origin = hit.position;
    newRay.direction = hit.normal * qSqrt(t) + hit.u * v * qCos(u) + hit.v * v * qSin(u);
    intensity = _diffuseColor;
}
