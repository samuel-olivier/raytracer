#ifndef INSTANCE_H
#define INSTANCE_H

#include "Node.h"

#include <QMatrix4x4>

class Instance : public Node
{
public:
    Instance(Node* object, Node* parent = 0);
    virtual ~Instance();

    virtual bool intersect(const Ray &ray, Intersection &hit);

    void    setMatrix(const QMatrix4x4 &matrix);
    void    setAnimation(const QMatrix4x4 &m1, const QMatrix4x4 &m2);

    virtual void    updateInfo();

private:
    void        _matrixAt(float t, QMatrix4x4 &m, QMatrix4x4 &invM) const;
    Node*       _object;
    bool        _isAnimated;
    QMatrix4x4  _m1;
    QMatrix4x4  _invM1;
    QMatrix4x4  _m2;
};

#endif // INSTANCE_H
