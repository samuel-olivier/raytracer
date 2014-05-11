#include "SphereSky.h"

#include <QtMath>

#include "ImageTexture.h"

SphereSky::SphereSky(QString const& path)
{
    setTexture(path);
}

SphereSky::SphereSky(ImageTexture *texture)
{
    setTexture(texture);
}

SphereSky::~SphereSky()
{
}

ImageTexture *SphereSky::texture() const
{
    return _texture;
}

void SphereSky::setTexture(ImageTexture *texture)
{
    if (_texture && _texture->hasImage()) {
        _texture = texture;
    } else {
        _texture = 0;
    }
}

void SphereSky::setTexture(const QString &path)
{
    _texture = new ImageTexture(path);
    if (!_texture->hasImage()) {
        delete _texture;
        _texture = 0;
    }
}

void SphereSky::evaluate(const QVector3D &direction, Color &color)
{
    if (_texture && _texture->hasImage()) {
        float x = (qAtan2(direction.x(), direction.z()) + M_PI) / (2 * M_PI),
                y = (qAsin(direction.y()) + M_PI_2) / M_PI;
        _texture->evaluateColor(x, y, color);
    } else {
        Sky::evaluate(direction, color);
    }
}
