#ifndef PLANE_H
#define PLANE_H

#include "Node.h"

#include <QVector3D>

class Plane : public Node
{
public:
    Plane(Node* parent = 0);
    virtual ~Plane();

    QVector3D const&    position() const;
    void    setPosition(QVector3D const& position);

    QVector3D const&    normal() const;
    void    setNormal(QVector3D const& normal);

    QVector3D const&    s() const;
    void    setS(QVector3D const& s);

    QVector3D const&    t() const;
    void    setT(QVector3D const& t);

    void    generateTangents();

    virtual bool    intersect(const Ray &ray, Intersection &hit);

    virtual void    updateInfo();

private:
    QVector3D   _position;
    QVector3D   _normal;
    QVector3D   _s;
    QVector3D   _t;
    QVector3D   _u;
    QVector3D   _v;
};

#endif // PLANE_H
