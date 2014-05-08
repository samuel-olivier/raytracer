#include "MetalMaterial.h"

#include <QDebug>
#include <QtMath>

#include "Config.h"

MetalMaterial::MetalMaterial()
    : _n(1.0f), _k(1.0f), _transmittedColor(Color::BLACK), _roughness(0.1f)
{
}

MetalMaterial::~MetalMaterial()
{
}

void MetalMaterial::computeReflectance(Color &col, const QVector3D &in, const Ray &ray, const Intersection &hit) const
{
    if (_roughness > 0) {
        float intensityValue = 0;
        QVector3D newDirection;
        Material::fresnelMetal(ray.direction, hit.normal, _n, _k, intensityValue, newDirection);
        col = Color::WHITE;
        col.Scale(Material::cookTorrance(-ray.direction, hit.normal, in, intensityValue, _roughness));
    } else {
        col = Color::BLACK;
    }
}

void MetalMaterial::sampleRay(const Ray &ray, const Intersection &hit, Ray &newRay, Color &intensity) const
{
    float intensityValue = 0;
    QVector3D newDirection;
    Material::fresnelMetal(ray.direction, hit.normal, _n, _k, intensityValue, newDirection);
    intensity = _transmittedColor;
    intensity.Scale(intensityValue);

    newRay.origin = hit.position;
    newRay.direction = newDirection;
}

float MetalMaterial::n() const
{
    return _n;
}

void MetalMaterial::setN(float n)
{
    _n = n;
}

float MetalMaterial::k() const
{
    return _k;
}

void MetalMaterial::setK(float k)
{
    _k = k;
}

const Color &MetalMaterial::transmittedColor() const
{
    return _transmittedColor;
}

void MetalMaterial::setTransmittedColor(const Color &transmittedColor)
{
    _transmittedColor = transmittedColor;
}

float MetalMaterial::roughness() const
{
    return _roughness;
}

void MetalMaterial::setRoughness(float roughness)
{
    _roughness = roughness;
}

