#ifndef TEXTURE_H
#define TEXTURE_H

#include <QVector2D>

#include "Color.h"

class Texture
{
public:
    enum WrappingMode {
        Wrap,
        Mirror,
        Clamp
    };

    Texture();
    virtual ~Texture();

    WrappingMode    wrappingModeX() const;
    void            setWrappingModeX(WrappingMode wrappingModeX);

    WrappingMode    wrappingModeY() const;
    void            setWrappingModeY(WrappingMode wrappingModeY);

    void            setWrappingModes(WrappingMode wrappingModeX, WrappingMode wrappingModeY);

    void    wrapCoordinates(float& x, float& y);
    void    wrapCoordinate(float& v, WrappingMode mode);

    virtual void    evaluateColor(float x, float y, Color& value);
    virtual void    evaluateFloat(float x, float y, float& value);
    virtual void    evaluateVec2(float x, float y, QVector2D &value);

    void    evaluateColor(QVector2D const& pos, Color& value);
    void    evaluateFloat(QVector2D const& pos, float& value);

private:
    WrappingMode    _wrappingModeX;
    WrappingMode    _wrappingModeY;
};

#endif // TEXTURE_H
