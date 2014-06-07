#include "Plane.h"

#include "Config.h"

Plane::Plane(Node *parent)
    : Node(parent), _normal(config->yAxis()), _s(1.0f, 0.0f, 0.0f), _t(0.0f, 0.0f, 1.0f)
{
    generateTangents();
}

Plane::~Plane()
{
}

const QVector3D &Plane::position() const
{
    return _position;
}

void Plane::setPosition(const QVector3D &position)
{
    _position = position;
}

const QVector3D &Plane::normal() const
{
    return _normal;
}

void Plane::setNormal(const QVector3D &normal)
{
    _normal = normal;
    generateTangents();
}

const QVector3D &Plane::s() const
{
    return _s;
}

void Plane::setS(const QVector3D &s)
{
    _s = s;
}

const QVector3D &Plane::t() const
{
    return _t;
}

void Plane::setT(const QVector3D &t)
{
    _t = t;
}

void Plane::generateTangents()
{
    _u = QVector3D::crossProduct(config->yAxis(), _normal);
    if (_u.length() < Config::Epsilon) {
        _u = config->xAxis();
    } else {
        _u.normalize();
    }
    _v = QVector3D::crossProduct(_normal, _u);
}

bool Plane::intersect(const Ray &ray, Intersection &hit)
{
    float den = QVector3D::dotProduct(ray.direction, _normal);
    if (den == 0) {
        return false;
    }
    float t = (QVector3D::dotProduct(_normal, _position) - QVector3D::dotProduct(ray.origin, _normal)) / den;
    if (t < Config::Epsilon) {
        return false;
    }
    if (t < hit.hitDistance) {
        if (ray.type == Ray::Shadow) {
            return true;
        }
        hit.hitDistance = t;
        hit.position = ray.origin + ray.direction * t;
        hit.normal = _normal;
        hit.material = material();

        QVector3D z = QVector3D::crossProduct(_s, _t);
        float zSquared = z.lengthSquared();
        QVector3D sp = QVector3D::crossProduct(_t, z) / zSquared;
        QVector3D tp = QVector3D::crossProduct(z, _s) / zSquared;
        hit.texCoord = QVector2D(QVector3D::dotProduct(hit.position - _position, sp),
                                 QVector3D::dotProduct(hit.position - _position, tp));
        hit.u = _u;
        hit.v = _v;
        hit.light = 0;
        return true;
    }
    return false;
}

void Plane::updateInfo()
{
    _boundingBox.setToInfinity();
    _center = QVector3D();
}

