#ifndef LAMBERTMATERIAL_H
#define LAMBERTMATERIAL_H

#include "Material.h"

class Texture;

class LambertMaterial : public Material
{
public:
    LambertMaterial();
    virtual ~LambertMaterial();

    Texture*        diffuseColor() const;
    void            setDiffuseColor(Texture* diffuseColor);
    void            setDiffuseColor(Color const& diffuseColor);

    virtual void    computeReflectance(Color &col, const QVector3D &in, const Ray &ray, const Intersection &hit) const;
    virtual bool    sampleRay(const Ray &ray, Intersection const& hit, Ray& newRay, Color& intensity) const;

private:
    Texture*    _diffuseColor;
};

#endif // LAMBERTMATERIAL_H
