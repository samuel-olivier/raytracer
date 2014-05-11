#include "Mesh.h"

#include "Triangle.h"
#include "Vertex.h"
#include "Config.h"

Mesh::Mesh(Node *parent) : Node(parent)
{
    setType("Mesh");
}

Mesh::~Mesh()
{
    for (Triangle* triangle : _triangles) {
        delete triangle;
    }
}

bool Mesh::intersect(const Ray &ray, Intersection &hit)
{
    bool success = false;
    for (Triangle* triangle : _triangles) {
        if (triangle->intersect(ray, hit)) {
            if (ray.type == Ray::Shadow) {
                return true;
            }
            success = true;
        }
    }
    return success;
}

const QVector<Triangle *> &Mesh::triangles() const
{
    return _triangles;
}

void Mesh::smooth()
{
//    int i;
//    for(i=0;i<_triangles.size();i++) {
//        _triangles[i]->v0->normal = QVector3D();
//        _triangles[i]->v1->normal = QVector3D();
//        _triangles[i]->v2->normal = QVector3D();
//    }
//    for(i=0;i<_triangles.size();i++) {
//        QVector3D cross = QVector3D::crossProduct(_triangles[i]->v1->position - _triangles[i]->v0->position, _triangles[i]->v2->position - _triangles[i]->v0->position);
//        _triangles[i]->v0->normal += cross;
//        _triangles[i]->v1->normal += cross;
//        _triangles[i]->v2->normal += cross;
//    }
//    for(i=0;i<_triangles.size();i++) {
//        _triangles[i]->v0->normal.normalize();
//        _triangles[i]->v1->normal.normalize();
//        _triangles[i]->v2->normal.normalize();
//    }

    for (int i = 0; i < _triangles.size(); ++i) {
        Triangle* tri = _triangles[i];
        tri->v0->normal = QVector3D(0.0f, 0.0f, 0.0f);
        tri->v1->normal = QVector3D(0.0f, 0.0f, 0.0f);
        tri->v2->normal = QVector3D(0.0f, 0.0f, 0.0f);
    }
    for (int i = 0; i < _triangles.size(); ++i) {
        Triangle* tri = _triangles[i];
        QVector3D cross = QVector3D::crossProduct((tri->v1->position - tri->v0->position).normalized(), (tri->v2->position - tri->v0->position).normalized()).normalized();
        tri->v0->normal += cross;
        tri->v1->normal += cross;
        tri->v2->normal += cross;
    }
    for (int i = 0; i < _triangles.size(); ++i) {
        Triangle* tri = _triangles[i];
        tri->v0->normal.normalize();
        tri->v1->normal.normalize();
        tri->v2->normal.normalize();
    }
}

void Mesh::generateTangents()
{
    for (int i = 0; i < _triangles.size(); ++i) {
        Triangle* tri = _triangles[i];
        tri->v0->generateTangents();
        tri->v1->generateTangents();
        tri->v2->generateTangents();
    }
}

void Mesh::setMaterial(Material *material)
{
    Node::setMaterial(material);
    for (Triangle* tri : _triangles) {
        tri->setMaterial(material);
    }
}

