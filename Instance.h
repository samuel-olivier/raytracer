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

    virtual void    updateInfo();

private:
    Node*       _object;
    QMatrix4x4  _matrix;
    QMatrix4x4  _invMatrix;
};

#endif // INSTANCE_H
