#include "Node.h"

#include <qmath.h>

#include "Mesh.h"
#include "BoxTreeNode.h"

Node::Node(Node *parent)
    : _parent(NULL), _visible(true), _material(0)
{
    setParent(parent);
    setType("Node");
    setName("Node");
}

Node::~Node()
{
    setParent(NULL);
    deleteChildren();
}

void Node::addChild(Node *child)
{
    _children.insert(child);
    child->_parent = this;
}

void Node::removeChild(Node *child)
{
    _children.remove(child);
    child->_parent = NULL;
}

const QSet<Node *> &Node::children() const
{
    return _children;
}

void Node::deleteChildren()
{
    for (auto it = _children.begin(); it != _children.end();) {
        Node* toDelete = *it;
        it = _children.erase(it);
        delete toDelete;
    }
}

void Node::setParent(Node *parent)
{
    if (_parent)
        _parent->removeChild(this);
    _parent = parent;
    if (_parent)
        _parent->addChild(this);
}

Node *Node::parent()
{
    return _parent;
}

const QString &Node::name()
{
    return _name;
}

void Node::setName(const QString &newName)
{
    _name = newName;
}

Node *Node::childByName(const QString &childName)
{
    if (childName == name())
        return this;
    foreach (Node* child, _children) {
        Node* found = child ? child->childByName(childName) : NULL;
        if (found)
            return found;
    }
    return NULL;
}

const QString &Node::type()
{
    return _type;
}

void Node::setType(const QString &type)
{
    _type = type;
}

bool Node::visible() const
{
    return _visible;
}

void Node::setVisible(bool isVisible)
{
    _visible = isVisible;
    foreach (Node* child, _children) {
        child->setVisible(isVisible);
    }
}

Material *Node::material() const
{
    return _material;
}

void Node::setMaterial(Material *material)
{
    _material = material;
}

bool Node::intersect(const Ray& /* ray */, Intersection& /* hit */)
{
    return false;
}

void Node::getMeshes(QList<Mesh*>& meshes) {
    Mesh* mesh = dynamic_cast<Mesh*>(this);

    if (mesh != 0) {
        meshes.append(mesh);
    }
    for (Node* child : _children) {
        child->getMeshes(meshes);
    }
}

const BoundingBox &Node::boundingBox() const
{
    return _boundingBox;
}

const QVector3D &Node::center() const
{
    return _center;
}

float Node::center(int dim) const
{
    return _center[dim];
}

void Node::updateInfo()
{
}
