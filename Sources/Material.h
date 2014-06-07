#ifndef MATERIAL_H
#define MATERIAL_H

#include <QVector3D>
#include <QString>
#include <QVector>
#include <QPair>

#include "Intersection.h"
#include "Color.h"
#include "Ray.h"

class Texture;

class Material
{
public:
    Material();
    virtual ~Material();

    virtual void computeReflectance(Color& col, const QVector3D& in, const Ray& ray, const Intersection& hit) const;
    virtual bool sampleRay(const Ray &ray, Intersection const& hit, Ray& newRay, Color& intensity) const;
    virtual void applyTransformation(Intersection& hit);

    static void fresnelDielectric(const QVector3D &d, const QVector3D &n, float ni, float nt, QVector<QPair<float, QVector3D> > &rays);
    static void fresnelMetal(const QVector3D &d, const QVector3D &n, float nt, float kt, float &intensity, QVector3D &rayDirection);
    static float cookTorrance(const QVector3D &v, const QVector3D &n, const QVector3D &L, float F, float m);
    static void  ashikhmin(QVector3D const& k1, QVector3D const& k2, QVector3D const& n, QVector3D const& u, QVector3D const& v, float rU, float rV, float s, float d, float& Ps, float& Pd);

    QString const&  name() const;
    void            setName(QString const& newName);

    Texture*    normalMap();
    void        setNormalMap(Texture* normalMap);

private:
    QString     _name;
    Texture*    _normalMap;
};

#endif // MATERIAL_H
