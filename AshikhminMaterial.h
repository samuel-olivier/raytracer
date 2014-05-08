#ifndef ASHIKHMINMATERIAL_H
#define ASHIKHMINMATERIAL_H

#include "Material.h"

class AshikhminMaterial : public Material
{
public:
    AshikhminMaterial();
    virtual ~AshikhminMaterial();

    float       diffuseLevel() const;
    void        setDiffuseLevel(float diffuseLevel);

    Color       diffuseColor() const;
    void        setDiffuseColor(Color const& diffuseColor);

    float       specularLevel() const;
    void        setSpecularLevel(float specularLevel);

    Color       specularColor() const;
    void        setSpecularColor(Color const& specularColor);

    QVector2D   roughness() const;
    float       roughnessU() const;
    float       roughnessV() const;
    void        setRoughness(float u, float v);
    void        setRoughness(QVector2D const& roughness);

    virtual void computeReflectance(Color& col, const QVector3D& in, const Ray& ray, const Intersection& hit) const;
    virtual void sampleRay(const Ray &ray, Intersection const& hit, Ray& newRay, Color& intensity) const;

private:
    float       _diffuseLevel;
    Color       _diffuseColor;
    float       _specularLevel;
    Color       _specularColor;
    QVector2D   _roughness;
};

#endif // ASHIKHMINMATERIAL_H
