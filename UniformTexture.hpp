#ifndef UNIFORMTEXTURE_HPP
#define UNIFORMTEXTURE_HPP

#include "Texture.h"

template<class T>
class UniformTexture : public Texture {
public:
    UniformTexture(T const& value) : _value(value) {}
    virtual ~UniformTexture() {}

    T const&    value() const { return _value; }
    void        setValue(T const& value) { _value = value; }

    virtual void    evaluateColor(float x, float y, Color &value) { Texture::evaluateColor(x, y, value); }
    virtual void    evaluateFloat(float x, float y, float &value) { Texture::evaluateFloat(x, y, value); }
    virtual void    evaluateVec2(float x, float y, QVector2D &value) { Texture::evaluateVec2(x, y, value); }

private:
    T   _value;
};

typedef UniformTexture<Color> UniformColor;
typedef UniformTexture<float> UniformFloat;
typedef UniformTexture<QVector2D> UniformVec2;

template<>
virtual void    UniformTexture<Color>::evaluateColor(float x, float y, Color &value) {value = _value; }

template<>
virtual void    UniformTexture<float>::evaluateFloat(float x, float y, float &value) {value = _value; }

template<>
virtual void    UniformTexture<QVector2D>::evaluateVec2(float x, float y, QVector2D &value) {value = _value; }

#endif // UNIFORMTEXTURE_HPP
