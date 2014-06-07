#include "Sphere.h"

#include <QtMath>
#include <QDebug>

#include "Config.h"

Sphere::Sphere(Node *parent)
    : Node(parent), _radius(0)
{
}

Sphere::~Sphere()
{
}

const QVector3D &Sphere::origin() const
{
    return _origin;
}

void Sphere::setOrigin(const QVector3D &position)
{
    _origin = position;
    updateInfo();
}

float Sphere::radius() const
{
    return _radius;
}

void Sphere::setRadius(float radius)
{
    _radius = radius;
    updateInfo();
}

bool Sphere::intersect(const Ray &ray, Intersection &hit)
{
    float t = -QVector3D::dotProduct(ray.origin - _origin, ray.direction);
    QVector3D q = ray.origin + ray.direction * t;
    float aSquared = _radius * _radius - (q - _origin).lengthSquared();
    if (aSquared < 0) {
        return false;
    }
    float a = qSqrt(aSquared);

    float dist;
    if (t - a > 0) {
        dist = t - a;
    } else if (t + a > 0) {
        dist = t + a;
    } else {
        return false;
    }
    if (dist > Config::Epsilon && dist < hit.hitDistance) {
        if (ray.type == Ray::Shadow) {
            return true;
        }
        hit.hitDistance = dist;
        hit.position = ray.origin + ray.direction * dist;
        hit.material = material();
        hit.normal = (hit.position - _origin) / _radius;
        hit.texCoord = QVector2D((qAtan2(hit.normal.x(), hit.normal.z()) + M_PI) / (2 * M_PI), (qAsin(hit.normal.y()) + M_PI_2) / M_PI);
        hit.u = QVector3D::crossProduct(config->yAxis(), hit.normal);
        if (hit.u.length() < Config::Epsilon) {
            hit.u = config->xAxis();
        } else {
            hit.u.normalize();
        }
        hit.v = QVector3D::crossProduct(hit.normal, hit.u);
        hit.light = 0;
        return true;
    }
    return false;
}

void Sphere::updateInfo()
{
    _center = _origin;
    _boundingBox = BoundingBox(_origin - QVector3D(_radius, _radius, _radius), _origin + QVector3D(_radius, _radius, _radius));
}
