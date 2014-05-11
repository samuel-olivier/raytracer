#include "Instance.h"

#include <QDebug>
#include <QtGlobal>

Instance::Instance(Node* object, Node *parent)
    : Node(parent), _object(object), _isAnimated(false)
{
}

Instance::~Instance()
{
}

bool Instance::intersect(const Ray &ray, Intersection &hit)
{
    if (_object) {
        QMatrix4x4 m, inv;

        _matrixAt(ray.time, m, inv);
        Ray ray2(inv.map(ray.origin),
                 inv.map(QVector4D(ray.direction, 0.0f)).toVector3D(),
                 ray.type);
        ray2.time = ray.time;
        if (_object->intersect(ray2, hit) == false) {
            return false;
        }
        if (ray.type == Ray::Shadow) {
            return true;
        }
        hit.position = m.map(hit.position);
        hit.normal = m.map(QVector4D(hit.normal, 0.0f)).toVector3D().normalized();
        hit.u = m.map(QVector4D(hit.u, 0.0f)).toVector3D().normalized();
        hit.v = m.map(QVector4D(hit.v, 0.0f)).toVector3D().normalized();
        hit.hitDistance = ray.origin.distanceToPoint(hit.position);
        if (_material) {
            hit.material = _material;
        }
        return true;
    }
    return false;
}

void Instance::setMatrix(QMatrix4x4 const& matrix)
{
    _m1 = matrix;
    _m1.setRow(3, QVector4D(0.0f, 0.0f, 0.0f, 1.0f));
    _invM1 = _m1.inverted();
    updateInfo();
}

void Instance::setAnimation(const QMatrix4x4 &m1, const QMatrix4x4 &m2)
{
    _m1 = m1;
    _m1.setRow(3, QVector4D(0.0f, 0.0f, 0.0f, 1.0f));
    _invM1 = _m1.inverted();
    _m2 = m2;
    _m2.setRow(3, QVector4D(0.0f, 0.0f, 0.0f, 1.0f));
    _isAnimated = true;
    updateInfo();
}

void Instance::updateInfo()
{
    if (_object) {
        if (_isAnimated) {
            BoundingBox const& b = _object->boundingBox();

            _boundingBox.reset();
            for (int i = 0; i < 128; ++i) {
                QMatrix4x4 m;
                QMatrix4x4 inv;
                _matrixAt(float(qrand()) / RAND_MAX, m, inv);
                _boundingBox.mergeTransformed(b, m);
            }

        } else {
            BoundingBox const& b = _object->boundingBox();

            _boundingBox.reset();
            _boundingBox.mergeTransformed(b, _m1);
        }
        _center = _boundingBox.center();
    } else {
        _boundingBox.reset();
        _center = QVector3D();
    }
}

void Instance::_matrixAt(float t, QMatrix4x4 &m, QMatrix4x4 &invM) const
{
    if (_isAnimated) {
        m = _m1 * t + _m2 * (1.0f - t);
        invM = m.inverted();
    } else {
        m = _m1;
        invM = _invM1;
    }
}
