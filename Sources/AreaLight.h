#ifndef AREALIGHT_H
#define AREALIGHT_H

#include "Light.h"

#include <QVector>
#include <QMatrix4x4>

class Triangle;

class AreaLight : public Light
{
public:
    AreaLight();
    virtual ~AreaLight();

    void    setTriangles(QVector<Triangle*> const& triangles);

    virtual bool  intersect(const Ray &ray, Intersection &hit);

    virtual float illuminate(const QVector3D& pos, Color& col, QVector3D& toLight, QVector3D& ltPos);
    virtual void  sampleRay(Ray& newRay, float& intensity, Color& color) const;

    virtual void  intersectionColor(Color& col);

    virtual int sampleNumber() const;
    void    transform(QMatrix4x4 const& mtx);

private:
    Triangle*   _sampleTriangle() const;

    QVector<Triangle*>  _triangles;
    QVector<float>      _areaCoef;
    float               _totalArea;
};

#endif // AREALIGHT_H
