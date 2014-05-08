#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>

#include "Ray.h"

class Camera
{
public:
    Camera();
    virtual ~Camera();

    void    set(float verticalFOV, float aspectRatio);
    void    ray(float x, float y, Ray &ray);
    void    lookAt(QVector3D const& position, QVector3D const& target, QVector3D const& up);

    void    setAperture(float aperture);
    void    setFocalPlane(float focalPlane);
private:
    float       _verticalFOV;
    float       _horizontalFOV;
    float       _aspectRatio;
    QVector3D   _screenTopLeft;
    float       _screenWidth;
    float       _screenHeight;
    QMatrix4x4  _matrix;
    float       _aperture;
    float       _focalPlane;
};

#endif // CAMERA_H
