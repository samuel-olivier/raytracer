#ifndef SPHERE_H
#define SPHERE_H

#include "Node.h"

class Sphere : public Node
{
public:
    Sphere(Node* parent = 0);
    virtual ~Sphere();

    QVector3D const&    origin() const;
    void                setOrigin(QVector3D const& origin);

    float               radius() const;
    void                setRadius(float radius);

    virtual bool        intersect(const Ray &ray, Intersection &hit);

    virtual void        updateInfo();

private:

    QVector3D   _origin;
    float       _radius;
};

#endif // SPHERE_H
