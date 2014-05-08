#include "Scene.h"

#include <QDebug>

#include "Node.h"
#include "Light.h"

Scene::Scene() : _root(new Node(NULL))
{
    _root->setName("Root");
    _root->setType("Scene");
}

Scene::~Scene()
{
}

const QList<Node *> &Scene::nodes() const
{
    return _nodes;
}

void Scene::addNode(Node *child)
{
    child->setParent(_root);
    _recursiveAdd(child);
}

const QList<Light *> &Scene::lights() const
{
    return _lights;
}

Node *Scene::root() const
{
    return _root;
}

void Scene::clear()
{
    delete _root;
    _root = new Node(NULL);
    _root->setName("Root");
}

const Color &Scene::skyColor() const
{
    return _skyColor;
}

void Scene::setSkyColor(const Color &skyColor)
{
    _skyColor = skyColor;
}

bool Scene::intersect(const Ray &ray, Intersection &hit)
{
    bool success = false;
    for (Node* node : _nodes) {
        if (node->intersect(ray, hit)) {
            if (ray.type == Ray::Shadow) {
                return true;
            }
            success = true;
        }
    }
    return success;
}

void Scene::_recursiveAdd(Node *current)
{
    if (!_nodes.contains(current)) {
        _nodes.prepend(current);
        Light* light = dynamic_cast<Light*>(current);
        if (light != 0) {
            _lights.prepend(light);
        }
    }
    for (Node* child : current->children()) {
        _recursiveAdd(child);
    }
}
