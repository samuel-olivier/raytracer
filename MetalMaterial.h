#ifndef METALMATERIAL_H
#define METALMATERIAL_H

#include "Material.h"

class MetalMaterial : public Material
{
public:
    MetalMaterial();
    virtual ~MetalMaterial();

    virtual void computeReflectance(Color &col, const QVector3D &in, const Ray& ray, const Intersection &hit) const;
    virtual void sampleRay(const Ray &ray, Intersection const& hit, Ray& newRay, Color& intensity) const;

    float   n() const;
    void    setN(float n);

    float   k() const;
    void    setK(float k);

    Color const& transmittedColor() const;
    void         setTransmittedColor(Color const& transmittedColor);

    float   roughness() const;
    void    setRoughness(float roughness);

private:
    float   _n;
    float   _k;
    Color   _transmittedColor;
    float   _roughness;
};

#endif // METALMATERIAL_H
