#ifndef DIELECTRICMATERIAL_H
#define DIELECTRICMATERIAL_H

#include "Material.h"

class DielectricMaterial : public Material
{
public:
    DielectricMaterial();
    virtual ~DielectricMaterial();

    virtual void    computeReflectance(Color &col, const QVector3D &in, const Ray &ray, const Intersection &hit) const;
    virtual void    sampleRay(const Ray &ray, Intersection const& hit, Ray& newRay, Color& intensity) const;

    float   n() const;
    void    setN(float n);

    float   absorptionCoef() const;
    void    setAbsorptionCoef(float absorptionCoef);

    Color const&   absorptionColor() const;
    void            setAbsorptionColor(Color const& absorptionColor);

    float   roughness() const;
    void    setRoughness(float roughness);

public:
    float   _n;
    float   _absorptionCoef;
    Color   _absorptionColor;
    float   _roughness;
};

#endif // DIELECTRICMATERIAL_H
