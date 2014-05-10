#include "Texture.h"

#include <QtMath>

Texture::Texture()
    : _wrappingModeX(Wrap), _wrappingModeY(Wrap)
{
}

Texture::~Texture()
{
}

Texture::WrappingMode Texture::wrappingModeX() const
{
    return _wrappingModeX;
}

void Texture::setWrappingModeX(Texture::WrappingMode wrappingModeX)
{
    _wrappingModeX = wrappingModeX;
}

Texture::WrappingMode Texture::wrappingModeY() const
{
    return _wrappingModeY;
}

void Texture::setWrappingModeY(Texture::WrappingMode wrappingModeY)
{
    _wrappingModeY = wrappingModeY;
}

void Texture::setWrappingModes(Texture::WrappingMode wrappingModeX, Texture::WrappingMode wrappingModeY)
{
    setWrappingModeX(wrappingModeX);
    setWrappingModeY(wrappingModeY);
}

void Texture::wrapCoordinates(float& x, float& y)
{
    wrapCoordinate(x, _wrappingModeX);
    wrapCoordinate(y, _wrappingModeY);
}

void Texture::wrapCoordinate(float &v, Texture::WrappingMode mode)
{
    if (mode == Wrap) {
        v = (v < 0.0f) ? (1.0f - fmodf(-v, 1.0f)) : fmodf(v, 1.0f);
    } else if (mode == Mirror) {
        v = (v < 0.0f) ? (2.0f - fmodf(-v, 2.0f)) : fmodf(v, 2.0f);
        if (v > 1.0f) {
            v = 2.0f - v;
        }
    } else if (mode == Clamp) {
        v = qMin(1.0f, qMax(0.0f, v));
    }
}

void Texture::evaluateColor(float, float, Color &value)
{
    value = Color::BLACK;
}

void Texture::evaluateColor(const QVector2D &pos, Color &value)
{
    evaluateColor(pos.x(), pos.y(), value);
}

void Texture::evaluateFloat(float, float, float &value)
{
    value = 0.0f;
}

void Texture::evaluateVec2(float x, float y, QVector2D &value)
{
    value = QVector2D();
}

void Texture::evaluateFloat(const QVector2D &pos, float &value)
{
    evaluateFloat(pos.x(), pos.y(), value);
}
