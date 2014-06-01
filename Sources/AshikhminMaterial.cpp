#include "AshikhminMaterial.h"

#include <QDebug>
#include <QtMath>
#include <QtGlobal>

#include "UniformTexture.hpp"

AshikhminMaterial::AshikhminMaterial()
{
    _diffuseLevel = new UniformFloat(1.0f);
    _diffuseColor = new UniformColor(Color::WHITE);
    _specularLevel = new UniformFloat(0.0f);
    _specularColor = new UniformColor(Color::WHITE);
    _roughnessU = new UniformFloat(500.0f);
    _roughnessV = new UniformFloat(500.0f);
    setType(Type(Type::Reflection | Type::Diffuse));
}

AshikhminMaterial::~AshikhminMaterial()
{
}

Texture* AshikhminMaterial::diffuseLevel() const
{
    return _diffuseLevel;
}

void AshikhminMaterial::setDiffuseLevel(float diffuseLevel)
{
    UniformFloat* t = dynamic_cast<UniformFloat*>(_diffuseLevel);
    if (t) {
        t->setValue(diffuseLevel);
    }
}

void AshikhminMaterial::setDiffuseLevel(Texture *diffuseLevel)
{
    _diffuseLevel = diffuseLevel;
}

Texture* AshikhminMaterial::diffuseColor() const
{
    return _diffuseColor;
}

void AshikhminMaterial::setDiffuseColor(const Color &diffuseColor)
{
    UniformColor* t = dynamic_cast<UniformColor*>(_diffuseColor);
    if (t) {
        t->setValue(diffuseColor);
    }
}

void AshikhminMaterial::setDiffuseColor(Texture *diffuseColor)
{
    _diffuseColor = diffuseColor;
}

Texture* AshikhminMaterial::specularLevel() const
{
    return _specularLevel;
}

void AshikhminMaterial::setSpecularLevel(float specularLevel)
{
    UniformFloat* t = dynamic_cast<UniformFloat*>(_specularLevel);
    if (t) {
        t->setValue(specularLevel);
    }
}

void AshikhminMaterial::setSpecularLevel(Texture *specularLevel)
{
    _specularLevel = specularLevel;
}

Texture* AshikhminMaterial::specularColor() const
{
    return _specularColor;
}

void AshikhminMaterial::setSpecularColor(const Color &specularColor)
{
    UniformColor* t = dynamic_cast<UniformColor*>(_specularColor);
    if (t) {
        t->setValue(specularColor);
    }
}

void AshikhminMaterial::setSpecularColor(Texture *specularColor)
{
    _specularColor = specularColor;
}

Texture* AshikhminMaterial::roughnessU() const
{
    return _roughnessU;
}

void AshikhminMaterial::setRoughnessU(float roughnessU)
{
    UniformFloat* t = dynamic_cast<UniformFloat*>(_roughnessU);
    if (t) {
        t->setValue(roughnessU);
    }
}

void AshikhminMaterial::setRoughnessU(Texture *roughnessU)
{
    _roughnessU = roughnessU;
}

Texture* AshikhminMaterial::roughnessV() const
{
    return _roughnessV;
}

void AshikhminMaterial::setRoughnessV(float roughnessV)
{
    UniformFloat* t = dynamic_cast<UniformFloat*>(_roughnessV);
    if (t) {
        t->setValue(roughnessV);
    }
}

void AshikhminMaterial::setRoughnessV(Texture *roughnessV)
{
    _roughnessV = roughnessV;
}

void AshikhminMaterial::setRoughness(float roughnessU, float roughnessV)
{
    setRoughnessU(roughnessU);
    setRoughnessV(roughnessV);
}

void AshikhminMaterial::setRoughness(Texture *roughnessU, Texture *roughnessV)
{
    setRoughnessU(roughnessU);
    setRoughnessV(roughnessV);
}

void AshikhminMaterial::computeReflectance(Color &col, const QVector3D &in, const Ray &ray, const Intersection &hit) const
{
    float Ps, Pd;
    float rU, rV;
    float sLevel, dLevel;
    Color sColor, dColor;

    _roughnessU->evaluateFloat(hit.texCoord, rU);
    _roughnessV->evaluateFloat(hit.texCoord, rV);
    _specularLevel->evaluateFloat(hit.texCoord, sLevel);
    _diffuseLevel->evaluateFloat(hit.texCoord, dLevel);
    _specularColor->evaluateColor(hit.texCoord, sColor);
    _diffuseColor->evaluateColor(hit.texCoord, dColor);

    Material::ashikhmin(in, -ray.direction, hit.normal, hit.u, hit.v, rU, rV, sLevel, dLevel, Ps, Pd);

    col = Color::BLACK;
    col.AddScaled(sColor, Ps);
    col.AddScaled(dColor, Pd);
    col.Scale(QVector3D::dotProduct(in, hit.normal));
}

bool AshikhminMaterial::sampleRay(const Ray &ray, const Intersection &hit, Ray &newRay, Color &intensity) const
{
    float sLevel;

    _specularLevel->evaluateFloat(hit.texCoord, sLevel);
    if (float(qrand()) / RAND_MAX < sLevel) {
        float rU, rV;
        Color sColor;

        _roughnessU->evaluateFloat(hit.texCoord, rU);
        _roughnessV->evaluateFloat(hit.texCoord, rV);
        _specularColor->evaluateColor(hit.texCoord, sColor);

        float E1 = float(qrand()) / RAND_MAX;
        float E2 = float(qrand()) / RAND_MAX;
        float phi = qAtan(qSqrt((rU + 1.0f) / (rV + 1.0f)) * qTan(M_PI * E1 * 0.5f));
        float quadrant = float(qrand()) / RAND_MAX;

        if (quadrant > 0.75f) {
            phi = 2.0f * M_PI - phi;
        } else if (quadrant > 0.5f) {
            phi = M_PI + phi;
        } else if (quadrant > 0.25f) {
            phi = M_PI - phi;
        }
        float cosTheta = qPow(1.0f - E2, 1.0f / (rU * qPow(qCos(phi), 2.0f) + rV * qPow(qSin(phi), 2.0f) + 1.0f));
        float sinTheta = qSin(qAcos(cosTheta));
        QVector3D h = hit.normal * cosTheta + hit.u * sinTheta * qCos(phi) + hit.v * sinTheta * qSin(phi);
        h.normalize();
        QVector3D k1 = -ray.direction;
        QVector3D k2 = -k1 + 2.0f * QVector3D::dotProduct(k1, h) * h;
        if (QVector3D::dotProduct(k2, hit.normal) < 0.0f) {
            intensity = Color::BLACK;
            return false;
        }
        k2.normalize();

        intensity = sColor;
        newRay.origin = hit.position;
        newRay.direction = k2;
    } else {
        float s = float(qrand()) / RAND_MAX;
        float t = float(qrand()) / RAND_MAX;
        float u = 2.0f * M_PI * s;
        float v = qSqrt(1.0f - t);
        Color dColor;

        _diffuseColor->evaluateColor(hit.texCoord, dColor);
        newRay.origin = hit.position;
        newRay.direction = hit.normal * qSqrt(t) + hit.u * v * qCos(u) + hit.v * v * qSin(u);
        newRay.direction.normalize();
        intensity = dColor;
    }
    return true;
}
