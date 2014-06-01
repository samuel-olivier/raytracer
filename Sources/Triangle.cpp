#include "Triangle.h"

#include <QDebug>
#include <QMatrix3x3>
#include <QtMath>

#include "Vertex.h"
#include "Material.h"
#include "Config.h"
#include "Texture.h"

Triangle::Triangle(Vertex *v0, Vertex *v1, Vertex *v2, Material *material) : Node(0)
{
    setMaterial(material);
    setVertices(v0, v1, v2);
}

Triangle::~Triangle()
{

}

void Triangle::setVertices(Vertex *v0, Vertex *v1, Vertex *v2)
{
    this->v0 = v0;
    this->v1 = v1;
    this->v2 = v2;
    updateInfo();
}

bool Triangle::intersect(const Ray &ray, Intersection &hit)
{
    if (!v0 || !v1 || !v2) {
        return false;
    }
    float detM = QVector3D::dotProduct(-ray.direction, QVector3D::crossProduct(v1->position - v0->position, v2->position - v0->position));
    if (detM == 0) {
        return false;
    }
    float alpha = QVector3D::dotProduct(-ray.direction, QVector3D::crossProduct(ray.origin - v0->position, v2->position - v0->position)) / detM;
    if (alpha < 0 || alpha > 1) {
        return false;
    }
    float beta = QVector3D::dotProduct(-ray.direction, QVector3D::crossProduct(v1->position - v0->position, ray.origin - v0->position)) / detM;
    if (beta < 0 || alpha + beta > 1) {
        return false;
    }
    float t = QVector3D::dotProduct(ray.origin - v0->position, QVector3D::crossProduct(v1->position - v0->position, v2->position - v0->position)) / detM;
    if (t > Config::Epsilon && t < hit.hitDistance - Config::Epsilon) {
        QVector2D texCoord = (1 - alpha - beta) * v0->texCoord + alpha * v1->texCoord + beta * v2->texCoord;
        if (_alphaMap) {
            float mean;
            _alphaMap->evaluateFloat(texCoord, mean);
            if (mean < 0.5f) {
                return false;
            }
        }
        if (ray.type == Ray::Shadow) {
            return true;
        }
        hit.hitDistance = t;
        hit.normal = (1 - alpha - beta) * v0->normal + alpha * v1->normal + beta * v2->normal;
        hit.texCoord = texCoord;
        hit.material = _material;
        hit.position = ray.origin + ray.direction * t;
        hit.u = (1 - alpha - beta) * v0->u + alpha * v1->u + beta * v2->u;
        hit.v = (1 - alpha - beta) * v0->v + alpha * v1->v + beta * v2->v;
        return true;
    }
    return false;
}

float Triangle::area() const
{
    if (!v0 || !v1 || !v2) {
        return 0;
    }
    return QVector3D::crossProduct(v1->position - v0->position, v2->position - v0->position).length() / 2.0f;
}

QVector3D Triangle::sample() const
{
    if (!v0 || !v1 || !v2) {
        return QVector3D();
    }
    float u = qSqrt(float(qrand()) / RAND_MAX),
          v = float(qrand()) / RAND_MAX;
    return v0->position + u * v * (v1->position - v0->position) + (1 - u) * (v2->position - v0->position);
}

QVector3D const& Triangle::normal() const
{
    return _normal;
}

void Triangle::transform(const QMatrix4x4 &mtx)
{
    v0->transform(mtx);
    v1->transform(mtx);
    v2->transform(mtx);
    updateInfo();
}

void Triangle::updateInfo()
{
    if (v0 && v1 && v2) {
        _normal = QVector3D::crossProduct(v1->position - v0->position, v2->position - v0->position).normalized();
        _center = (v0->position + v1->position + v2->position) / 3.0f;
        float xMin = qMin(v0->position.x(), qMin(v1->position.x(), v2->position.x()));
        float yMin = qMin(v0->position.y(), qMin(v1->position.y(), v2->position.y()));
        float zMin = qMin(v0->position.z(), qMin(v1->position.z(), v2->position.z()));
        float xMax = qMax(v0->position.x(), qMax(v1->position.x(), v2->position.x()));
        float yMax = qMax(v0->position.y(), qMax(v1->position.y(), v2->position.y()));
        float zMax = qMax(v0->position.z(), qMax(v1->position.z(), v2->position.z()));
        _boundingBox = BoundingBox(QVector3D(xMin, yMin, zMin), QVector3D(xMax, yMax, zMax));
    } else {
        _normal = QVector3D();
        _center = _normal;
        _boundingBox.reset();
    }
}
