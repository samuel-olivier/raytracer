#include "BoxTreeNode.h"

#include <QDebug>

#include <algorithm>

#include "Mesh.h"
#include "Triangle.h"
#include "Config.h"

class NodeComp {
public:
    NodeComp(int dim)
        : _dim(dim)
    {
    }

    bool    operator()(Node* n1, Node* n2)
    {
        if (!n1 || !n2) {
            false;
        }
        QVector3D g1 = n1->center();
        QVector3D g2 = n2->center();
        return g1[_dim] < g2[_dim];
    }

private:
    int _dim;
};

int treeHeight;
int leafNumber;
int nodeNumber;

BoxTreeNode::BoxTreeNode(Node *parent)
    : Node(parent), _child1(0), _child2(0)
{
}

BoxTreeNode::~BoxTreeNode()
{
}

void BoxTreeNode::construct(Mesh *mesh)
{
    if (mesh != 0) {
        QVector<Triangle*> const& triangles = mesh->triangles();
        QVector<Node*> nodes(triangles.size());
        for (int i = 0; i < triangles.size(); ++i) {
            nodes[i] = triangles[i];
        }
        construct(nodes);
    }
}

void BoxTreeNode::construct(const QVector<Node *> &nodes)
{
    treeHeight = 0;
    leafNumber = 0;
    nodeNumber = 0;
    _construct(nodes);
//    qDebug() << "";
//    qDebug() << "Tree height" << treeHeight;
//    qDebug() << "Node number" << nodeNumber;
//    qDebug() << "Leaf number" << leafNumber;
//    qDebug() << "";
}

void BoxTreeNode::_construct(const QVector<Node *> &nodes, int currentDeepness)
{
    nodeNumber++;
    for (int i = 0; i < nodes.size(); ++i) {
        _boundingBox.merge(nodes[i]->boundingBox());
    }
    _center = _boundingBox.center();
    if (nodes.size() <= config->maximumNodesPerBox()) {
        _nodes = nodes;
        treeHeight = qMax(currentDeepness, treeHeight);
        leafNumber++;
        return ;
    }

    QVector3D boxSize = _boundingBox.size();

    _axis = 2;
    if (boxSize.x() > boxSize.y() && boxSize.x() > boxSize.z()) {
        _axis = 0;
    } else if (boxSize.y() > boxSize.x() && boxSize.y() > boxSize.z()) {
        _axis = 1;
    }
    float split = _boundingBox.center()[_axis];
    QVector<Node*> t1;
    QVector<Node*> t2;
    t1.reserve(nodes.size());
    t2.reserve(nodes.size());

    for (Node* node : nodes) {
        float g = node->center(_axis);
        if (g > split) {
            t2.append(node);
        } else {
            t1.append(node);
        }
    }
    if (t1.isEmpty() || t2.isEmpty()) {
        t1 = nodes;
        NodeComp comp(_axis);
        std::nth_element(t1.begin(), t1.begin() + (t1.size() / 2), t1.end(), comp);
        t2 = t1.mid(t1.size() / 2);
        t1.resize(t1.size() / 2);
    }
    t1.squeeze();
    t2.squeeze();

//    qDebug() << "Axe : " << _axis << " | " << _boundingBox.min << " | " << _boundingBox.max << " | " << _boundingBox.size() << t1.size() << t2.size();

    _child1 = new BoxTreeNode;
    _child1->_construct(t1, currentDeepness + 1);

    _child2 = new BoxTreeNode;
    _child2->_construct(t2, currentDeepness + 1);
}

bool BoxTreeNode::intersect(const Ray &ray, Intersection &hit)
{
    if (_boundingBox.intersect(ray, hit.hitDistance) == false) {
        return false;
    } else if (_child1 && _child2) {
        BoxTreeNode* first = _child1;
        BoxTreeNode* second = _child2;
        bool success = false;

        if (ray.direction[_axis] < 0) {
            qSwap(first, second);
        }
        if (first->intersect(ray, hit)) {
            if (ray.type == Ray::Shadow) {
                return true;
            }
            success = true;
        }
        if (second->intersect(ray, hit)) {
            if (ray.type == Ray::Shadow) {
                return true;
            }
            success = true;
        }
        return success;
    } else {
        bool success = false;
        for (int i = 0; i < _nodes.size(); ++i) {
            if (_nodes[i]->intersect(ray, hit)) {
                if (ray.type == Ray::Shadow) {
                    return true;
                }
                success = true;
            }
        }
        return success;
    }
}
