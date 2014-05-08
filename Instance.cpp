#include "Instance.h"

#include <QDebug>

Instance::Instance(Node* object, Node *parent)
    : Node(parent), _object(object)
{
}

Instance::~Instance()
{
}

bool Instance::intersect(const Ray &ray, Intersection &hit)
{
    if (_object) {
        Ray ray2(_invMatrix.map(ray.origin),
                 _invMatrix.map(QVector4D(ray.direction, 0.0f)).toVector3D(),
                 ray.type);
        if (_object->intersect(ray2, hit) == false) {
            return false;
        }
        if (ray.type == Ray::Shadow) {
            return true;
        }
        hit.position = _matrix.map(hit.position);
        hit.normal = _matrix.map(QVector4D(hit.normal, 0.0f)).toVector3D().normalized();
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
    _matrix = matrix;
    _matrix.setRow(3, QVector4D(0.0f, 0.0f, 0.0f, 1.0f));
    _invMatrix = _matrix.inverted();
    updateInfo();
}

void Instance::updateInfo()
{
    if (_object) {
        BoundingBox const& b = _object->boundingBox();

        _boundingBox.reset();
        _boundingBox.merge(_matrix.map(QVector3D(b.min[0], b.min[1], b.min[2])));
        _boundingBox.merge(_matrix.map(QVector3D(b.min[0], b.min[1], b.max[2])));
        _boundingBox.merge(_matrix.map(QVector3D(b.min[0], b.max[1], b.min[2])));
        _boundingBox.merge(_matrix.map(QVector3D(b.min[0], b.max[1], b.max[2])));
        _boundingBox.merge(_matrix.map(QVector3D(b.max[0], b.min[1], b.min[2])));
        _boundingBox.merge(_matrix.map(QVector3D(b.max[0], b.min[1], b.max[2])));
        _boundingBox.merge(_matrix.map(QVector3D(b.max[0], b.max[1], b.min[2])));
        _boundingBox.merge(_matrix.map(QVector3D(b.max[0], b.max[1], b.max[2])));
        _center = _matrix.map(_object->center());
    } else {
        _boundingBox.reset();
        _center = QVector3D();
    }
}
