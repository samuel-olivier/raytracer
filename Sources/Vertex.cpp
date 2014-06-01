#include "Vertex.h"

#include <QDebug>

#include "Config.h"

Vertex::Vertex(QVector3D const&position, QVector3D const&normal, QVector2D const&texCoord)
{
    set(position, normal, texCoord);
}

void Vertex::set(QVector3D const&position, QVector3D const&normal, QVector2D const&texCoord)
{
    this->position = position;
    this->normal = normal;
    this->texCoord = texCoord;
}

void Vertex::generateTangents()
{
    u = QVector3D::crossProduct(config->yAxis(), normal);
    if (u.length() < Config::Epsilon) {
        u = config->xAxis();
    } else {
        u.normalize();
    }
    v = QVector3D::crossProduct(normal, u);
}

void Vertex::transform(const QMatrix4x4 &mtx)
{
    position = mtx.map(position);
    normal = mtx.map(QVector4D(normal, 0.0f)).toVector3D();
}
