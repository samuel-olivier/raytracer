#include "CustomMaterial.h"

#include <QColor>
#include <QDebug>
#include <QtMath>
#include <QtGlobal>
#include <QMatrix4x4>

#include "Logger.h"

CustomMaterial::CustomMaterial()
    : _diffuseColor(Color::WHITE), _hasTexture(false)
{
    setName("Custom Material");
}

CustomMaterial::~CustomMaterial()
{
}

const Color &CustomMaterial::diffuseColor() const
{
    return _diffuseColor;
}

void CustomMaterial::setDiffuseColor(const Color &diffuseColor)
{
    _diffuseColor = diffuseColor;
}

bool CustomMaterial::hasTexture() const
{
    return _hasTexture;
}

void CustomMaterial::setHasTexture(bool hasTexture)
{
    _hasTexture = hasTexture;
}

const QImage &CustomMaterial::texture() const
{
    return _texture;
}

void CustomMaterial::setTexture(const QImage &texture)
{
    _texture = texture;
    _hasTexture = true;
}

void CustomMaterial::loadTexture(const QString &filename)
{
    _hasTexture = _texture.load(filename);
    if (_hasTexture == false) {
        logger->writeError(QString("Could not load texture \"%1\"").arg(filename));
    }
}

void CustomMaterial::computeReflectance(Color &col, const QVector3D &in, const Ray &, const Intersection &hit) const
{
    if (_hasTexture && !_texture.isNull()) {
        int x = int(hit.texCoord.x() * (_texture.width() - 1)) % _texture.width();
        float y = int(hit.texCoord.y() * (_texture.height() - 1)) % _texture.height();
        if (x < 0) {
            x += _texture.width();
        }
        if (y < 0) {
            y += _texture.height();
        }
        QColor color = _texture.pixel(x, _texture.height() - y - 1);
        col.Set(color.redF(), color.greenF(), color.blueF());
    } else {
        col = _diffuseColor;
    }
    col.Scale(QVector3D::dotProduct(in, hit.normal));
}

void CustomMaterial::sampleRay(const Ray &, const Intersection &hit, Ray &newRay, Color &intensity) const
{
    float s = float(qrand()) / RAND_MAX;
    float t = float(qrand()) / RAND_MAX;
    float u = 2.0f * M_PI * s;
    float v = qSqrt(1.0f - t);
    newRay.origin = hit.position;
    newRay.direction = hit.normal * qSqrt(t) + hit.u * v * qCos(u) + hit.v * v * qSin(u);
    intensity = _diffuseColor;
}
