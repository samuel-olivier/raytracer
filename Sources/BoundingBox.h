#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <QVector3D>
#include <QMatrix4x4>

#include "Ray.h"

class BoundingBox
{
public:

    enum Axis {
        X,
        Y,
        Z
    };

    BoundingBox();
    BoundingBox(QVector3D const& min, QVector3D const& max);
    ~BoundingBox();

    void    merge(BoundingBox const& other);
    void    merge(QVector3D const& point);
    void    mergeTransformed(BoundingBox const& other, QMatrix4x4 const& m);

    void    reset();
    void    setToInfinity();

    bool intersect(Ray const& ray, float maxDistance) const;

    float   surfaceArea() const;

    QVector3D   center() const;
    QVector3D   size() const;

    QVector3D   min;
    QVector3D   max;
};

#endif // BOUNDINGBOX_H
