#include "DielectricMaterial.h"

#include <QtMath>
#include <QDebug>

#include "Config.h"

DielectricMaterial::DielectricMaterial()
    : _n(1.0003f), _absorptionCoef(0.0f), _absorptionColor(Color::WHITE), _roughness(0.2f)
{
    setType(Type(Type::Reflection | Type::Transmission));
    setName("Dielectric Material");
}

DielectricMaterial::~DielectricMaterial()
{
}

void DielectricMaterial::computeReflectance(Color &col, const QVector3D &in, const Ray &ray, const Intersection &hit) const
{
    float ni = config->refractionIndex();
    float nt = _n;

    if (ray.refraction != config->refractionIndex()) {
        qSwap(ni, nt);
    }

    QVector<QPair<float, QVector3D>> rays;
    col = Color::BLACK;
    Material::fresnelDielectric(ray.direction, hit.normal, ni, nt, rays);
    if (_roughness > 0.0f && rays.size() > 0) {
        col = Color::WHITE;
        col.Scale(Material::cookTorrance(-ray.direction, hit.normal, in, rays.first().first, _roughness));
    }
}

bool DielectricMaterial::sampleRay(const Ray &ray, const Intersection &hit, Ray &newRay, Color &intensity) const
{
    float ni = config->refractionIndex();
    float nt = _n;
    float absorption = 0.0f;

    if (ray.refraction != config->refractionIndex()) {
        qSwap(ni, nt);
        absorption = _absorptionCoef;
    }

    QVector<QPair<float, QVector3D>> rays;
    Material::fresnelDielectric(ray.direction, hit.normal, ni, nt, rays);
    float s = float(qrand()) / RAND_MAX;
    if (rays.size() == 1 || (rays.size() == 2 && s < rays.first().first)) {
        QPair<float, QVector3D> const& ray1 = rays.first();
        intensity = Color::WHITE;
        newRay.origin = hit.position;
        newRay.direction = ray1.second;
        newRay.refraction = ni;
    } else if (rays.size() == 2) {
        QPair<float, QVector3D> const& ray2 = rays.at(1);
        intensity = Color::WHITE;
        newRay.origin = hit.position;
        newRay.direction = ray2.second;
        newRay.refraction = nt;
        if (absorption > 0.0f) {
            float s = -absorption * hit.position.distanceToPoint(ray.origin);
            intensity.Red *= qExp(s * (1.0f - _absorptionColor.Red));
            intensity.Green *= qExp(s * (1.0f - _absorptionColor.Green));
            intensity.Blue *= qExp(s * (1.0f - _absorptionColor.Blue));
        }
    } else {
        intensity = Color::BLACK;
        return false;
    }
    return true;
}

float DielectricMaterial::n() const
{
    return _n;
}

void DielectricMaterial::setN(float n)
{
    _n = n;
}

float DielectricMaterial::absorptionCoef() const
{
    return _absorptionCoef;
}

void DielectricMaterial::setAbsorptionCoef(float absorptionCoef)
{
    _absorptionCoef = absorptionCoef;
}

const Color &DielectricMaterial::absorptionColor() const
{
    return _absorptionColor;
}

void DielectricMaterial::setAbsorptionColor(const Color &absorptionColor)
{
    _absorptionColor = absorptionColor;
}

float DielectricMaterial::roughness() const
{
    return _roughness;
}

void DielectricMaterial::setRoughness(float roughness)
{
    _roughness = roughness;
}
