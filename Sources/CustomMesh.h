#ifndef CUSTOMMESH_H
#define CUSTOMMESH_H

#include "Mesh.h"

class CustomMesh : public Mesh
{
public:
    CustomMesh(Node* parent = 0);
    virtual ~CustomMesh();

    void    setTriangles(QVector<Triangle*> const& triangles);
};

#endif // CUSTOMMESH_H
