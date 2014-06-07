#ifndef RAY_H
#define RAY_H

#include <QVector3D>

class Ray
{

public:
    enum Type {
        Primary,
        Shadow,
        Reflected,
        Transmitted,
        Diffused
    };

    Ray();
    Ray(QVector3D const& origin, QVector3D const& direction, Type type = Primary);

    void        computeInverse();

    QVector3D   origin;
    QVector3D   direction;
    Type        type;
    QVector3D   inverseDirection;
    int         depth;
    float       refraction;
    float       time;
};

#endif // RAY_H
