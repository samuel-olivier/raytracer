#include "BoundingBox.h"

#include <QDebug>

#include "Config.h"

BoundingBox::BoundingBox()
{
    reset();
}

BoundingBox::BoundingBox(const QVector3D &min, const QVector3D &max)
    : min(min), max(max)
{
}

BoundingBox::~BoundingBox()
{
}

void BoundingBox::merge(const BoundingBox &other)
{
    min.setX(qMin(min.x(), other.min.x()));
    min.setY(qMin(min.y(), other.min.y()));
    min.setZ(qMin(min.z(), other.min.z()));

    max.setX(qMax(max.x(), other.max.x()));
    max.setY(qMax(max.y(), other.max.y()));
    max.setZ(qMax(max.z(), other.max.z()));
}

void BoundingBox::merge(const QVector3D &point)
{
    min.setX(qMin(min.x(), point.x()));
    min.setY(qMin(min.y(), point.y()));
    min.setZ(qMin(min.z(), point.z()));

    max.setX(qMax(max.x(), point.x()));
    max.setY(qMax(max.y(), point.y()));
    max.setZ(qMax(max.z(), point.z()));
}

void BoundingBox::reset()
{
    min = QVector3D(std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max());
    max = QVector3D(1-std::numeric_limits<float>::max(),
                    1-std::numeric_limits<float>::max(),
                    1-std::numeric_limits<float>::max());
}

void BoundingBox::setToInfinity()
{
    min = QVector3D(1-std::numeric_limits<float>::max(),
                    1-std::numeric_limits<float>::max(),
                    1-std::numeric_limits<float>::max());
    max = QVector3D(std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max());
}

bool BoundingBox::intersect(const Ray &ray, float maxDistance) const
{
    float tx1 = (min.x() - ray.origin.x()) / ray.direction.x(),
            tx2 = (max.x() - ray.origin.x()) / ray.direction.x(),
            ty1 = (min.y() - ray.origin.y()) / ray.direction.y(),
            ty2 = (max.y() - ray.origin.y()) / ray.direction.y(),
            tz1 = (min.z() - ray.origin.z()) / ray.direction.z(),
            tz2 = (max.z() - ray.origin.z()) / ray.direction.z();

    float tMin = qMax(qMax(qMin(tx1, tx2), qMin(ty1, ty2)), qMin(tz1, tz2)),
            tMax = qMin(qMin(qMax(tx1, tx2), qMax(ty1, ty2)), qMax(tz1, tz2));

    if (tMin <= tMax) {
        if ((tMin <= Config::Epsilon && tMax <= Config::Epsilon) || tMin >= maxDistance - Config::Epsilon) {
            return false;
        }
        return true;
    }
    return false;
}

QVector3D BoundingBox::center() const
{
    return (min + max) * 0.5f;
}

QVector3D BoundingBox::size() const
{
    return max - min;
}
