#ifndef LAMBERTMATERIAL_H
#define LAMBERTMATERIAL_H

#include "Material.h"


class LambertMaterial : public Material
{
public:
    LambertMaterial();
    virtual ~LambertMaterial();

    Color const&    diffuseColor() const;
    void            setDiffuseColor(Color const& diffuseColor);

    virtual void    computeReflectance(Color &col, const QVector3D &in, const Ray &ray, const Intersection &hit) const;
    virtual void    sampleRay(const Ray &ray, Intersection const& hit, Ray& newRay, Color& intensity) const;

private:
    Color  _diffuseColor;
};

#endif // LAMBERTMATERIAL_H
