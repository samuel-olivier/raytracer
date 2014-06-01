#ifndef PLASTICMATERIAL_H
#define PLASTICMATERIAL_H

#include "Material.h"

class PlasticMaterial : public Material
{
public:
    PlasticMaterial();
    virtual ~PlasticMaterial();

    virtual void computeReflectance(Color &col, const QVector3D &in, const Ray &ray, const Intersection &hit) const;
    virtual bool sampleRay(const Ray &ray, Intersection const& hit, Ray& newRay, Color& intensity) const;

    Color const&    diffuseColor() const;
    void            setDiffuseColor(Color const& diffuseColor);

    float   n() const;
    void    setN(float n);

    float   roughness() const;
    void    setRoughness(float roughness);

private:
    Color   _diffuseColor;
    float   _n;
    float   _roughness;
};

#endif // PLASTICMATERIAL_H
