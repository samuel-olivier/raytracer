#include "CustomMesh.h"

CustomMesh::CustomMesh(Node* parent)
    : Mesh(parent)
{
}

CustomMesh::~CustomMesh()
{

}

void CustomMesh::setTriangles(const QVector<Triangle *> &triangles)
{
    _triangles = triangles;
    generateTangents();
}
