#include "Material.h"

#include <QDebug>
#include <QtMath>

#include "Config.h"
#include "Texture.h"

Material::Material() : _normalMap(0), _type(Type::Diffuse)
{
    setName("Material");
}

Material::~Material()
{
}

void Material::computeReflectance(Color &col, const QVector3D &, const Ray&, const Intersection &) const
{
    col = Color::BLACK;
}

bool Material::sampleRay(const Ray &, const Intersection &hit, Ray &newRay, Color &intensity) const
{
    newRay.origin = hit.position;
    newRay.direction = hit.normal;
    intensity = Color::BLACK;
    return false;
}

void Material::applyTransformation(Intersection &hit)
{
    if (_normalMap) {
        Color col;
        _normalMap->evaluateColor(hit.texCoord, col);
        QVector3D n(col.Red - 0.5f, col.Blue - 0.5f, col.Green - 0.5f);
        hit.normal = hit.normal * n.y() + hit.u * n.z() + hit.v * n.x();
        hit.normal.normalize();
    }
}

void Material::fresnelDielectric(const QVector3D &d, const QVector3D &normal, float ni, float nt, QVector<QPair<float, QVector3D> > &rays)
{
    QVector3D n = normal;
    float ndd = QVector3D::dotProduct(n, d);
    if (ndd > -Config::Epsilon && ndd < Config::Epsilon) {
        return ;
    }
    if (ndd > 0) {
        n *= -1;
        ndd *= -1;
    }
    QVector3D r = d - 2.0f * ndd * n;
    r.normalize();

    QVector3D z = (d - ndd * n) * ni / nt;
    float z2 = z.lengthSquared();

    if (z2 >= 1.0f) {
        rays.append(QPair<float, QVector3D>(1.0f, r));
        return ;
    }

    QVector3D t = z - qSqrt(1.0f - z2) * n;
    t.normalize();

    float ndt = QVector3D::dotProduct(n, t);
    float rPar = (nt * ndd - ni * ndt) / (nt * ndd + ni * ndt);
    float rPerp = (ni * ndd - nt * ndt) / (ni * ndd + nt * ndt);
    float fr = 0.5f * (rPar * rPar + rPerp * rPerp);

    rays.append(QPair<float, QVector3D>(fr, r));
    rays.append(QPair<float, QVector3D>(1.0f - fr, t));
}

void Material::fresnelMetal(const QVector3D &d, const QVector3D &n, float nt, float kt, float& intensity, QVector3D& rayDirection)
{
    float ndd = QVector3D::dotProduct(n, d);
    if (ndd > 0) {
        intensity = 1.0;
        rayDirection = d;
        return ;
    }
    float nd2 = ndd * ndd;
    float nk2 = nt * nt + kt * kt;
    float nnd2 = 2.0f * nt * ndd;

    float rPar = (nk2 * nd2 + nnd2 + 1.0f) / (nk2 * nd2 - nnd2 + 1.0f);
    float rPerp = (nk2 + nd2 + nnd2) / (nk2 + nd2 - nnd2);

    intensity = 0.5f * (rPar + rPerp);
    rayDirection = d - 2.0f * ndd * n;
    rayDirection.normalize();
}

float Material::cookTorrance(const QVector3D &v, const QVector3D &n, const QVector3D &L, float F, float m)
{
    QVector3D vL = v + L;
    QVector3D h = vL / vL.length();
    float nh = QVector3D::dotProduct(n, h);
    float nv = QVector3D::dotProduct(n, v);
    float vh = QVector3D::dotProduct(v, h);
    float nL = QVector3D::dotProduct(n, L);
    float m2 = m * m;
    float alpha = qMin(0.999, qAcos(nh));
    float G = qMin(1.0f, qMin(2.0f * nh * nv / vh, 2.0f * nh * nL / vh));
    float D = qExp(-qPow(qTan(alpha), 2.0f) / m2) / (m2 * qPow(qCos(alpha), 4.0f));
    return qMin(24.0f, F * G * D / (4.0f * float(M_PI) * nL * nv));
}

void Material::ashikhmin(const QVector3D &k1, const QVector3D &k2, const QVector3D &nTmp, const QVector3D &u, const QVector3D &v, float rU, float rV, float s, float d, float &Ps, float &Pd)
{
    QVector3D h = (k1 + k2).normalized();
    QVector3D n = nTmp;

    float kh = QVector3D::dotProduct(k2, h);
    float nk1 = QVector3D::dotProduct(k1, n);
    if (nk1 < 0) {
        nk1 *= -1;
        n *= -1;
    }
    float nk2 = QVector3D::dotProduct(k2, n);
    float nh = QVector3D::dotProduct(n, h);
    float hu = QVector3D::dotProduct(h, u);
    float hv = QVector3D::dotProduct(h, v);

    float F = s + (1.0f - s) * qPow(1.0f - kh, 5.0f);

    float Ps1 = qSqrt((rU + 1) * (rV + 1)) / (8.0f * M_PI);
    float Ps2 = qPow(nh, ((rU * hu * hu + rV * hv * hv) / (1.0f - nh * nh))) / (kh * qMax(nk1, nk2));
    Ps = qMin(24.0f, Ps1 * Ps2 * F);

    float Pd1 = ((28.0f * d) / (23.0f * M_PI)) * (1.0f - s);
    float Pd2 = 1.0f - qPow(1.0f - 0.5f * nk1, 5.0f);
    float Pd3 = 1.0f - qPow(1.0f - 0.5f * nk2, 5.0f);
    Pd = Pd1 * Pd2 * Pd3;
}

QString const& Material::name() const
{
    return _name;
}

void Material::setName(const QString &name)
{
    _name = name;
}

Texture *Material::normalMap()
{
    return _normalMap;
}

void Material::setNormalMap(Texture *normalMap)
{
    _normalMap = normalMap;
}

Material::Type Material::type() const
{
    return _type;
}

bool Material::isType(Material::Type test) const
{
    return _type & test;
}

void Material::setType(Material::Type type)
{
    _type = type;
}
