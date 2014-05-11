#ifndef SCENE_H
#define SCENE_H

#include <QList>

#include "Color.h"
#include "Ray.h"
#include "Intersection.h"

class Node;
class Light;
class Sky;

class Scene
{
public:
    Scene();
    ~Scene();

    QList<Node*> const&  nodes() const;
    QList<Light*> const&  lights() const;
    void    addNode(Node* child);

    Node*   root() const;

    void    clear();

    Sky*    sky() const;
    void    setSky(Sky* sky);

    bool            intersect(Ray const& ray, Intersection& hit);

private:
    void    _recursiveAdd(Node* current);

    Node*           _root;
    Sky*            _sky;
    QList<Node*>    _nodes;
    QList<Light*>   _lights;
};

#endif // SCENE_H
