#ifndef LIGHT_H
#define LIGHT_H

#include "Node.h"

#include <QVector3D>

#include "Color.h"

class Light : public Node
{
public:
    Light();
    virtual ~Light();

    Color const&    baseColor() const;
    void            setBaseColor(Color const& baseColor);

    float   intensity() const;
    void    setIntensity(float intensity);

    virtual float illuminate(const QVector3D& pos, Color& col, QVector3D& toLight, QVector3D& ltPos) = 0;
    virtual void  sampleRay(Ray& newRay, float& intensity, Color& color) const = 0;

    virtual void  intersectionColor(Color& col);

    virtual int sampleNumber() const = 0;

private:
    Color   _baseColor;
    float   _intensity;
};

#endif // LIGHT_H
