#include "Camera.h"

#include <QtMath>
#include <QVector3D>
#include <QDebug>

#include "Config.h"

Camera::Camera()
{
    _aperture = 0.0f;
    _focalPlane = 2.5f;
}

Camera::~Camera()
{
}

void Camera::set(float verticalFOV, float aspectRatio)
{
    _verticalFOV = qDegreesToRadians(verticalFOV);
    _aspectRatio = aspectRatio;
    _screenHeight = qTan(_verticalFOV / 2.f);
    _horizontalFOV = 2.f * qAtan(_aspectRatio * _screenHeight);
    _screenHeight *= 2.f;
    _screenWidth = _screenHeight * _aspectRatio;
    _screenTopLeft = (-_matrix.column(2) - (_screenWidth / 2.f) * _matrix.column(0) + (_screenHeight / 2.f) * _matrix.column(1) + _matrix.column(3)).toVector3D();
}

void Camera::ray(float x, float y, Ray& ray)
{
    float s = float(qrand()) / RAND_MAX;
    float t = float(qrand()) / RAND_MAX;
    float u = 2.0f * M_PI * s;

    ray.origin = (_matrix.column(3) + qSqrt(t) * qCos(u) * _aperture * _matrix.column(0) + qSqrt(t) * qSin(u) * _aperture * _matrix.column(1)).toVector3D();

    QVector3D direction = (_screenTopLeft + x * _screenWidth * _matrix.column(0) + y * _screenHeight * _matrix.column(1) - _matrix.column(3)).toVector3D();
    QVector3D focus = _matrix.column(3).toVector3D() + direction * _focalPlane;
    ray.direction = focus - ray.origin;
    ray.direction.normalize();
}

void Camera::lookAt(const QVector3D &position, const QVector3D &target, const QVector3D &up)
{
    QVector3D d = position;
    QVector3D c = d - target;
    c.normalize();
    QVector3D a = QVector3D::crossProduct(up, c);
    a.normalize();
    QVector3D b = QVector3D::crossProduct(c, a);
    _matrix.setColumn(0, a);
    _matrix.setColumn(1, b);
    _matrix.setColumn(2, c);
    _matrix.setColumn(3, d);
}

void Camera::setAperture(float aperture)
{
    _aperture = aperture;
}

void Camera::setFocalPlane(float focalPlane)
{
    _focalPlane = focalPlane;
}
