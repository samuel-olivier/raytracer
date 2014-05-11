#ifndef CUSTOMMATERIAL_H
#define CUSTOMMATERIAL_H

#include "Material.h"

#include <QImage>

#include "Color.h"

class CustomMaterial : public Material
{
public:
    CustomMaterial();
    virtual ~CustomMaterial();

    Color const&    diffuseColor() const;
    void            setDiffuseColor(Color const& diffuseColor);

    bool    hasTexture() const;
    void    setHasTexture(bool hasTexture);

    QImage const&   texture() const;
    void            setTexture(QImage const& texture);
    void            loadTexture(QString const& filename);

    virtual void    computeReflectance(Color &col, const QVector3D &in, const Ray &ray, const Intersection &hit) const;
    virtual void    sampleRay(const Ray &ray, Intersection const& hit, Ray& newRay, Color& intensity) const;

private:
    Color   _diffuseColor;
    bool    _hasTexture;
    QImage  _texture;
};

#endif // CUSTOMMATERIAL_H
