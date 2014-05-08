#include "AshikhminMaterial.h"

#include <QDebug>
#include <QtMath>
#include <QtGlobal>

AshikhminMaterial::AshikhminMaterial()
{
    _diffuseLevel = 1.0f;
    _diffuseColor = Color::BLACK;
    _specularLevel = 0.0f;
    _specularColor = Color::BLACK;
}

AshikhminMaterial::~AshikhminMaterial()
{
}

float AshikhminMaterial::diffuseLevel() const
{
    return _diffuseLevel;
}

void AshikhminMaterial::setDiffuseLevel(float diffuseLevel)
{
    _diffuseLevel = diffuseLevel;
}

Color AshikhminMaterial::diffuseColor() const
{
    return _diffuseColor;
}

void AshikhminMaterial::setDiffuseColor(const Color &diffuseColor)
{
    _diffuseColor = diffuseColor;
}

float AshikhminMaterial::specularLevel() const
{
    return _specularLevel;
}

void AshikhminMaterial::setSpecularLevel(float specularLevel)
{
    _specularLevel = specularLevel;
}

Color AshikhminMaterial::specularColor() const
{
    return _specularColor;
}

void AshikhminMaterial::setSpecularColor(const Color &specularColor)
{
    _specularColor = specularColor;
}

QVector2D AshikhminMaterial::roughness() const
{
    return _roughness;
}

float AshikhminMaterial::roughnessU() const
{
    return _roughness.x();
}

float AshikhminMaterial::roughnessV() const
{
    return _roughness.y();
}

void AshikhminMaterial::setRoughness(float u, float v)
{
    _roughness.setX(u);
    _roughness.setY(v);
}

void AshikhminMaterial::setRoughness(const QVector2D &roughness)
{
    _roughness = roughness;
}

void AshikhminMaterial::computeReflectance(Color &col, const QVector3D &in, const Ray &ray, const Intersection &hit) const
{
    float Ps, Pd;

    Material::ashikhmin(in, -ray.direction, hit.normal, hit.u, hit.v, roughnessU(), roughnessV(), _specularLevel, _diffuseLevel, Ps, Pd);
    col = Color::BLACK;
    col.AddScaled(_specularColor, Ps);
    col.AddScaled(_diffuseColor, Pd);
    col.Scale(QVector3D::dotProduct(in, hit.normal));
}

void AshikhminMaterial::sampleRay(const Ray &ray, const Intersection &hit, Ray &newRay, Color &intensity) const
{
    if (float(qrand()) / RAND_MAX < _specularLevel) {
        float E1 = float(qrand()) / RAND_MAX;
        float E2 = float(qrand()) / RAND_MAX;
        float phi = qAtan(qSqrt((roughnessU() + 1.0f) / (roughnessV() + 1.0f)) * qTan(M_PI * E1 * 0.5f));
        float quadrant = float(qrand()) / RAND_MAX;

        if (quadrant > 0.75f) {
            phi = 2.0f * M_PI - phi;
        } else if (quadrant > 0.5f) {
            phi = M_PI + phi;
        } else if (quadrant > 0.25f) {
            phi = M_PI - phi;
        }
        float cosTheta = qPow(1.0f - E2, 1.0f / (roughnessU() * qPow(qCos(phi), 2.0f) + roughnessV() * qPow(qSin(phi), 2.0f) + 1.0f));
        float sinTheta = qSin(qAcos(cosTheta));
        QVector3D h = hit.normal * cosTheta + hit.u * sinTheta * qCos(phi) + hit.v * sinTheta * qSin(phi);
        h.normalize();
        QVector3D k1 = -ray.direction;
        QVector3D k2 = -k1 + 2.0f * QVector3D::dotProduct(k1, h) * h;
        k2.normalize();

        intensity = _specularColor;
        newRay.origin = hit.position;
        newRay.direction = k2;
    } else {
        float s = float(qrand()) / RAND_MAX;
        float t = float(qrand()) / RAND_MAX;
        float u = 2.0f * M_PI * s;
        float v = qSqrt(1.0f - t);

        newRay.origin = hit.position;
        newRay.direction = hit.normal * qSqrt(t) + hit.u * v * qCos(u) + hit.v * v * qSin(u);
        newRay.direction.normalize();
        intensity = _diffuseColor;
    }
}
