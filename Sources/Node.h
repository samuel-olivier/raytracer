#ifndef NODE_H
#define NODE_H

#include <QSet>
#include <QHash>
#include <QString>

#include "Ray.h"
#include "Intersection.h"
#include "BoundingBox.h"

class Mesh;

class Node
{
public:
    Node(Node* parent = 0);
    virtual ~Node();

    void    addChild(Node* child);
    void    removeChild(Node* child);
    QSet<Node*> const& children() const;
    void    deleteChildren();

    void            setParent(Node* parent);
    Node*           parent();

    QString const&  name();
    void            setName(QString const& newName);
    Node*           childByName(QString const& childName);

    QString const&  type();
    void            setType(QString const& type);

    bool            visible() const;
    void            setVisible(bool isVisible);

    Material*       material() const;
    virtual void    setMaterial(Material* material);

    virtual bool    intersect(const Ray &ray, Intersection &hit);

    void            getMeshes(QList<Mesh*>& meshes);

    BoundingBox const&  boundingBox() const;

    QVector3D const&    center() const;
    float               center(int dim) const;

    virtual void        updateInfo();


protected:
    QSet<Node*>     _children;
    Node*           _parent;
    QString         _name;
    QString         _type;
    bool            _visible;
    Material*       _material;
    BoundingBox     _boundingBox;
    QVector3D       _center;
};

#endif // NODE_H
