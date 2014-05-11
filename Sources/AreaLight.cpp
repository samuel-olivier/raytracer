#include "AreaLight.h"

#include <QDebug>

#include "Config.h"
#include "Triangle.h"

AreaLight::AreaLight()
{
}

AreaLight::~AreaLight()
{
}

void AreaLight::setTriangles(const QVector<Triangle *> &triangles)
{
    _triangles = triangles;
    _areaCoef.resize(_triangles.size());

    _totalArea = 0;
    for (int i = 0; i < triangles.size(); ++i) {
        _areaCoef[i] = triangles[i]->area();
        _totalArea += _areaCoef[i];
    }
    for (int i = 0; i < triangles.size(); ++i) {
        _areaCoef[i] /= _totalArea;
    }
}

bool AreaLight::intersect(const Ray &ray, Intersection &hit)
{
    bool success = false;
    for (Triangle* triangle : _triangles) {
        if (triangle->intersect(ray, hit)) {
            if (ray.type == Ray::Shadow) {
                return true;
            }
            success = true;
        }
    }
    if (success) {
        hit.light = this;
    }
    return success;
}

float AreaLight::illuminate(const QVector3D &pos, Color &col, QVector3D &toLight, QVector3D &ltPos)
{
    Triangle* t = _sampleTriangle();
    if (t == 0) {
        return 0;
    }
    ltPos = t->sample();
    toLight = ltPos - pos;
    float bright = intensity() / (_totalArea * toLight.lengthSquared());
    toLight.normalize();
    col = baseColor();
    float cosA = QVector3D::dotProduct(toLight, -t->normal());
    return bright * qMax(cosA, 0.0f);
}

void AreaLight::intersectionColor(Color &col)
{
    col = baseColor();
    col.Scale(intensity() / _totalArea);
}

int AreaLight::sampleNumber() const
{
    return config->lightSampleNumber();
}

Triangle *AreaLight::_sampleTriangle()
{
    float s = float(qrand()) / RAND_MAX;
    float sum = 0;
    for (int i = 0; i < _areaCoef.size(); ++i) {
        if (s <= sum + _areaCoef[i]) {
            return _triangles[i];
        } else {
            sum += _areaCoef[i];
        }
    }
    return 0;
}
