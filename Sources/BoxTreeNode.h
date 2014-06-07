#ifndef BOXTREENODE_H
#define BOXTREENODE_H

#include "Node.h"

#include <QVector>

#include "BoundingBox.h"

class Mesh;
class Node;

class BoxTreeNode : public Node
{
public:
    BoxTreeNode(Node* parent = 0);
    virtual ~BoxTreeNode();

    void    construct(Mesh* mesh);
    void    construct(QVector<Node*> const& nodes);

    virtual bool    intersect(const Ray &ray, Intersection &hit);

private:
    void    _construct(QVector<Node*> const& nodes, int currentDeepness = 0, bool computeBoundingBox = true);

    BoxTreeNode*        _child1;
    BoxTreeNode*        _child2;
    QVector<Node*>      _nodes;
    int                 _axis;
};

#endif // BOXTREENODE_H
