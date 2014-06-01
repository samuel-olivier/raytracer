#ifndef MESH_H
#define MESH_H

#include "Node.h"

#include <QVector>

class Triangle;
class Vertex;

class Mesh : public Node
{
public:
    Mesh(Node* parent = 0);
    virtual ~Mesh();

    virtual bool        intersect(const Ray &ray, Intersection &hit);

    QVector<Triangle*> const&   triangles() const;

    void    smooth();
    void    generateTangents();
    void    generateTextureTangents();

    virtual void    setMaterial(Material* material);
    virtual void    setAlphaMap(Texture* alphaMap);

protected:
    QVector<Vertex*>    _vertices;
    QVector<Triangle*>  _triangles;
};

#endif // MESH_H
