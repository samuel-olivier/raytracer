#include "Box.h"

#include <QVector2D>

#include "Config.h"
#include "Triangle.h"
#include "Vertex.h"

Box::Box(Node *parent) : Mesh(parent)
{
}

void Box::set(float x, float y, float z)
{
    qDeleteAll(_triangles);
    qDeleteAll(_vertices);
    _triangles.resize(12);
    _vertices.resize(24);

    x*=0.5f;
    y*=0.5f;
    z*=0.5f;

    // Corners
    QVector3D p000(-x, -y, -z);
    QVector3D p001(-x, -y, z);
    QVector3D p010(-x, y, -z);
    QVector3D p011(-x, y, z);
    QVector3D p100(x, -y, -z);
    QVector3D p101(x, -y, z);
    QVector3D p110(x, y, -z);
    QVector3D p111(x, y, z);

    // Tex coords
    QVector2D t00(0.0f, 0.0f);
    QVector2D t01(0.0f, 1.0f);
    QVector2D t10(1.0f, 0.0f);
    QVector2D t11(1.0f, 1.0f);

    // Right
    _vertices[0] = new Vertex(p101, config->xAxis(), t00);
    _vertices[1] = new Vertex(p100, config->xAxis(), t10);
    _vertices[2] = new Vertex(p110, config->xAxis(), t11);
    _vertices[3] = new Vertex(p111, config->xAxis(), t01);
    _triangles[0] = new Triangle(_vertices[0], _vertices[1], _vertices[2], material());
    _triangles[1] = new Triangle(_vertices[0], _vertices[2], _vertices[3], material());

    // Left
    _vertices[4] = new Vertex(p000, -config->xAxis(), t00);
    _vertices[5] = new Vertex(p001, -config->xAxis(), t10);
    _vertices[6] = new Vertex(p011, -config->xAxis(), t11);
    _vertices[7] = new Vertex(p010, -config->xAxis(), t01);
    _triangles[2] = new Triangle(_vertices[4], _vertices[5], _vertices[6], material());
    _triangles[3] = new Triangle(_vertices[4], _vertices[6], _vertices[7], material());

    // Top
    _vertices[8] = new Vertex(p011, config->yAxis(), t00);
    _vertices[9] = new Vertex(p111, config->yAxis(), t10);
    _vertices[10] = new Vertex(p110, config->yAxis(), t11);
    _vertices[11] = new Vertex(p010, config->yAxis(), t01);
    _triangles[4] = new Triangle(_vertices[8], _vertices[9], _vertices[10], material());
    _triangles[5] = new Triangle(_vertices[8], _vertices[10], _vertices[11], material());

    // Bottom
    _vertices[12] = new Vertex(p000, -config->yAxis(), t00);
    _vertices[13] = new Vertex(p100, -config->yAxis(), t10);
    _vertices[14] = new Vertex(p101, -config->yAxis(), t11);
    _vertices[15] = new Vertex(p001, -config->yAxis(), t01);
    _triangles[6] = new Triangle(_vertices[12], _vertices[13], _vertices[14], material());
    _triangles[7] = new Triangle(_vertices[12], _vertices[14], _vertices[15], material());

    // Front
    _vertices[16] = new Vertex(p001, config->zAxis(), t00);
    _vertices[17] = new Vertex(p101, config->zAxis(), t10);
    _vertices[18] = new Vertex(p111, config->zAxis(), t11);
    _vertices[19] = new Vertex(p011, config->zAxis(), t01);
    _triangles[8] = new Triangle(_vertices[16], _vertices[17], _vertices[18], material());
    _triangles[9] = new Triangle(_vertices[16], _vertices[18], _vertices[19], material());

    // Back
    _vertices[20] = new Vertex(p100, -config->zAxis(), t00);
    _vertices[21] = new Vertex(p000, -config->zAxis(), t10);
    _vertices[22] = new Vertex(p010, -config->zAxis(), t11);
    _vertices[23] = new Vertex(p110, -config->zAxis(), t01);
    _triangles[10] = new Triangle(_vertices[20], _vertices[21], _vertices[22], material());
    _triangles[11] = new Triangle(_vertices[20], _vertices[22], _vertices[23], material());

    _boundingBox = BoundingBox(p000, p111);
    _center = _boundingBox.center();
    generateTextureTangents();
}
