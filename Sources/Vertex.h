#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>
#include <QVector2D>
#include <QMatrix4x4>

class Vertex
{
public:
    Vertex(QVector3D const&position = QVector3D(), QVector3D const&normal = QVector3D(), QVector2D const&texCoord = QVector2D());

    void set(const QVector3D &position, const QVector3D &normal, const QVector2D &texCoord);
    void generateTangents();
    void transform(const QMatrix4x4 &mtx);

    QVector3D position;
    QVector3D normal;
    QVector3D u;
    QVector3D v;
    QVector2D texCoord;
};

#endif // VERTEX_H
