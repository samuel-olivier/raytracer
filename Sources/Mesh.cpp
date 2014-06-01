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

        QVector2D F = tri->v1->texCoord - tri->v0->texCoord;
        QVector2D G = tri->v2->texCoord- tri->v0->texCoord;
        if (F.lengthSquared() == 0.0f || G.lengthSquared() == 0.0f ) {
            tri->v0->generateTangents();
            tri->v1->generateTangents();
            tri->v2->generateTangents();
        } else {
            QVector3D D = tri->v1->position - tri->v0->position;
            QVector3D E = tri->v2->position - tri->v0->position;
            QMatrix2x2 GF;
            GF.m[0][0] = G.y();
            GF.m[0][1] = -F.y();
            GF.m[1][0] = -G.x();
            GF.m[1][1] = F.x();
            QMatrix3x2 DE;
            DE.m[0][0] = D.x();
            DE.m[0][1] = D.y();
            DE.m[0][2] = D.z();
            DE.m[1][0] = E.x();
            DE.m[1][1] = E.y();
            DE.m[1][2] = E.z();
            QMatrix3x2 m = 1.0f / (F.x() * G.y() - F.y() * G.x()) * (GF * DE);
            QVector3D u(m.m[0][0], m.m[0][1], m.m[0][2]);
            QVector3D v(m.m[1][0], m.m[1][1], m.m[1][2]);

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

