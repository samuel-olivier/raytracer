#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <QVector3D>
#include <QVector2D>

#include "Color.h"

class Material;
class Node;
class Light;

class Intersection
{
public:
    Intersection();

    float       hitDistance;
    QVector3D   position;
    QVector3D   normal;
    QVector2D   texCoord;
    Material*   material;
    Color       shade;
    Light*      light;
    QVector3D   u;
    QVector3D   v;
};

#endif // INTERSECTION_H
