#include "AreaLight.h"

#include <QDebug>
#include <QtMath>

#include "Config.h"
#include "Triangle.h"
#include "Vertex.h"

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
        Triangle* triangle = triangles[i];
        _areaCoef[i] = triangle->area();
        _totalArea += _areaCoef[i];
        triangle->v0->u = (triangle->v1->position - triangle->v0->position).normalized();
        triangle->v0->v = QVector3D::crossProduct(triangle->v0->u, triangle->normal());
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

void AreaLight::sampleRay(Ray &newRay, float &intensity, Color &color) const
{
    Triangle* tri = _sampleTriangle();
    if (tri == 0) {
        intensity = 0.0f;
        color = Color::BLACK;
    } else {
        float s = float(qrand()) / RAND_MAX;
        float t = float(qrand()) / RAND_MAX;
        float u = 2.0f * M_PI * s;
        float v = qSqrt(1.0f - t);

        intensity = this->intensity() / _totalArea;
        color = baseColor();
        newRay.direction = tri->normal() * qSqrt(t) + tri->v0->u * v * qCos(u) + tri->v0->v * v * qSin(u);
        newRay.direction.normalize();
        newRay.origin = tri->sample();
    }
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

void AreaLight::transform(const QMatrix4x4 &mtx)
{
    for (Triangle* tri : _triangles) {
        tri->transform(mtx);
    }
    setTriangles(_triangles);
}

Triangle *AreaLight::_sampleTriangle() const
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
