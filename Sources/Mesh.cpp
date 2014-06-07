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
    if (success) {
        hit.light = 0;
    }
    return success;
}

const QVector<Triangle *> &Mesh::triangles() const
{
    return _triangles;
}

void Mesh::smooth()
{
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

void Mesh::generateTextureTangents()
{
    for (int i = 0; i < _triangles.size(); ++i) {
        Triangle* tri = _triangles[i];
        tri->v0->u = QVector3D(0.0f, 0.0f, 0.0f);
        tri->v1->u = QVector3D(0.0f, 0.0f, 0.0f);
        tri->v2->u = QVector3D(0.0f, 0.0f, 0.0f);
        tri->v0->v = QVector3D(0.0f, 0.0f, 0.0f);
        tri->v1->v = QVector3D(0.0f, 0.0f, 0.0f);
        tri->v2->v = QVector3D(0.0f, 0.0f, 0.0f);
    }
    for (int i = 0; i < _triangles.size(); ++i) {
        Triangle* tri = _triangles[i];

        QVector2D t = tri->v1->texCoord - tri->v0->texCoord;
        QVector2D s = tri->v2->texCoord - tri->v0->texCoord;
        if (s.lengthSquared() == 0.0f || t.lengthSquared() == 0.0f ) {
            tri->v0->generateTangents();
            tri->v1->generateTangents();
            tri->v2->generateTangents();
        } else {
            QVector3D v2 = tri->v1->position - tri->v0->position;
            QVector3D v1 = tri->v2->position - tri->v0->position;

            float r = 1.0f / (s.x() * t.y() - s.y() * t.x());
            QVector3D u((t.y() * v1.x() - t.x() * v2.x()) * r, (t.y() * v1.y() - t.x() * v2.y()) * r,
                            (t.y() * v1.z() - t.x() * v2.z()) * r);
            QVector3D v((s.x() * v2.x() - s.y() * v1.x()) * r, (s.x() * v2.y() - s.y() * v1.y()) * r,
                            (s.x() * v2.z() - s.y() * v1.z()) * r);
            u.normalize();
            v.normalize();
            tri->v0->u += u;
            tri->v1->u += u;
            tri->v2->u += u;
            tri->v0->v += v;
            tri->v1->v += v;
            tri->v2->v += v;
        }
    }
    for (int i = 0; i < _triangles.size(); ++i) {
        Triangle* tri = _triangles[i];
        tri->v0->u.normalize();
        tri->v1->u.normalize();
        tri->v2->u.normalize();
        tri->v0->v.normalize();
        tri->v1->v.normalize();
        tri->v2->v.normalize();
    }

}

void Mesh::setMaterial(Material *material)
{
    Node::setMaterial(material);
    for (Triangle* tri : _triangles) {
        tri->setMaterial(material);
    }
}

void Mesh::setAlphaMap(Texture *alphaMap)
{
    Node::setAlphaMap(alphaMap);
    for (Triangle* tri : _triangles) {
        tri->setAlphaMap(alphaMap);
    }
}

