#ifndef ASHIKHMINMATERIAL_H
#define ASHIKHMINMATERIAL_H

#include "Material.h"

class Texture;

class AshikhminMaterial : public Material
{
public:
    AshikhminMaterial();
    virtual ~AshikhminMaterial();

    Texture*    diffuseLevel() const;
    void        setDiffuseLevel(float diffuseLevel);
    void        setDiffuseLevel(Texture* diffuseLevel);

    Texture*    diffuseColor() const;
    void        setDiffuseColor(Color const& diffuseColor);
    void        setDiffuseColor(Texture* diffuseColor);

    Texture*    specularLevel() const;
    void        setSpecularLevel(float specularLevel);
    void        setSpecularLevel(Texture* specularLevel);

    Texture*    specularColor() const;
    void        setSpecularColor(Color const& specularColor);
    void        setSpecularColor(Texture* specularColor);

    Texture*    roughnessU() const;
    void        setRoughnessU(float roughnessU);
    void        setRoughnessU(Texture* roughnessU);

    Texture*    roughnessV() const;
    void        setRoughnessV(float roughnessV);
    void        setRoughnessV(Texture* roughnessV);

    void        setRoughness(float roughnessU, float roughnessV);
    void        setRoughness(Texture* roughnessU, Texture* roughnessV);

    virtual void computeReflectance(Color& col, const QVector3D& in, const Ray& ray, const Intersection& hit) const;
    virtual void sampleRay(const Ray &ray, Intersection const& hit, Ray& newRay, Color& intensity) const;

private:
    Texture*    _diffuseLevel;
    Texture*    _diffuseColor;
    Texture*    _specularLevel;
    Texture*    _specularColor;
    Texture*    _roughnessU;
    Texture*    _roughnessV;
};

#endif // ASHIKHMINMATERIAL_H
