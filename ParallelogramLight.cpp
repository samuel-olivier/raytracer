#include "ParallelogramLight.h"

#include "Triangle.h"
#include "Vertex.h"

ParallelogramLight::ParallelogramLight()
{
}

ParallelogramLight::~ParallelogramLight()
{
}

void ParallelogramLight::set(const QVector3D &origin, const QVector3D &u, const QVector3D &v)
{
    QVector3D p0 = origin,
            p1 = origin + u,
            p2 = origin + v,
            p3 = origin + u + v,
            n = QVector3D::crossProduct(u, v).normalized();

    Vertex* v0 = new Vertex(p0, n);
    Vertex* v1 = new Vertex(p1, n);
    Vertex* v2 = new Vertex(p2, n);
    Vertex* v3 = new Vertex(p3, n);

    Triangle* t0 = new Triangle(v0, v1, v3);
    Triangle* t1 = new Triangle(v0, v3, v2);

    QVector<Triangle*> triangles(2);
    triangles[0] = t0;
    triangles[1] = t1;
    setTriangles(triangles);
}
