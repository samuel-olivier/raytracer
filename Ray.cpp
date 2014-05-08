#include "Ray.h"

#include "Config.h"

Ray::Ray()
    : type(Primary), depth(0), refraction(config->refractionIndex())
{
}

Ray::Ray(const QVector3D &origin, const QVector3D &direction, Type type)
    : origin(origin), direction(direction), type(type), depth(0), refraction(config->refractionIndex())
{
    computeInverse();
}

void Ray::computeInverse()
{
    inverseDirection = QVector3D(1.0f / direction.x(), 1.0f / direction.y(), 1.0f / direction.z());
}
