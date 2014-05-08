#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Node.h"

#include "Ray.h"
#include "Intersection.h"

class Vertex;

class Triangle : public Node
{
public:
    Triangle(Vertex* v0 = 0, Vertex* v1 = 0, Vertex* v2 = 0, Material* material = 0);
    ~Triangle();

    void        setVertices(Vertex* v0, Vertex* v1, Vertex* v2);

    bool intersect(const Ray &ray, Intersection &hit);

    float       area() const;
    QVector3D   sample() const;

    QVector3D const& normal() const;

    virtual void    updateInfo();

    Vertex*     v0;
    Vertex*     v1;
    Vertex*     v2;

private:
    QVector3D   _normal;

};

#endif // TRIANGLE_H
