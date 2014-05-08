#include "PlasticMaterial.h"

#include <QDebug>

#include "Config.h"

PlasticMaterial::PlasticMaterial()
    : _diffuseColor(Color::WHITE), _n(1.0003f), _roughness(0.2f)
{
}

PlasticMaterial::~PlasticMaterial()
{
}

void PlasticMaterial::computeReflectance(Color &col, const QVector3D &in, const Ray& ray, const Intersection &hit) const
{
    float ni = config->refractionIndex();
    float nt = _n;

    QVector<QPair<float, QVector3D>> rays;
    Material::fresnelDielectric(ray.direction, hit.normal, ni, nt, rays);
    col = _diffuseColor;
    if (rays.size() > 1) {
        col.Scale(rays.at(1).first * QVector3D::dotProduct(hit.normal, in));
    } else {
        col.Scale(0.0f);
    }
    if (rays.size() > 0 && _roughness > 0.0f) {
        Color spec = Color::WHITE;
        spec.Scale(Material::cookTorrance(-ray.direction, hit.normal, in, rays.first().first, _roughness));
        col.Add(spec);
    }
}

void PlasticMaterial::sampleRay(const Ray &ray, const Intersection &hit, Ray &newRay, Color &intensity) const
{
    float ni = config->refractionIndex();
    float nt = _n;

    QVector<QPair<float, QVector3D>> rays;
    Material::fresnelDielectric(ray.direction, hit.normal, ni, nt, rays);

    if (rays.size() > 0) {
        intensity = Color::WHITE;
        intensity.Scale(rays.first().first);
        newRay.origin = hit.position;
        newRay.direction = rays.first().second;
    }
}

void PlasticMaterial::computeRays(const Ray &ray, const Intersection &hit, QVector<QPair<Color, Ray> > &newRays) const
{
    float ni = config->refractionIndex();
    float nt = _n;

    QVector<QPair<float, QVector3D>> rays;
    Material::fresnelDielectric(ray.direction, hit.normal, ni, nt, rays);

    if (rays.size() > 0) {
        Color c = Color::WHITE;
        c.Scale(rays.first().first);
        newRays.append(QPair<Color, Ray>(c, Ray(hit.position, rays.first().second)));
    }
}

const Color &PlasticMaterial::diffuseColor() const
{
    return _diffuseColor;
}

void PlasticMaterial::setDiffuseColor(const Color &diffuseColor)
{
    _diffuseColor = diffuseColor;
}

float PlasticMaterial::n() const
{
    return _n;
}

void PlasticMaterial::setN(float n)
{
    _n = n;
}

float PlasticMaterial::roughness() const
{
    return _roughness;
}

void PlasticMaterial::setRoughness(float roughness)
{
    _roughness = roughness;
}
