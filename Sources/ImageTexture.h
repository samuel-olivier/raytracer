#ifndef IMAGETEXTURE_H
#define IMAGETEXTURE_H

#include "Texture.h"

#include <QString>
#include <QImage>
#include <QColor>

class ImageTexture : public Texture
{
public:
    ImageTexture(QString const& path = QString(), float scaleFactor = 1.0f);
    virtual ~ImageTexture();

    bool            hasImage() const;

    const QImage&   image() const;
    void            setImage(const QImage &image);
    bool            loadImage(QString const& filename);

    virtual void    evaluateColor(float x, float y, Color& value);
    virtual void    evaluateFloat(float x, float y, float& value);

    float           scaleFactor() const;
    void            setScaleFactor(float scaleFactor);

private:
    void    _pixelColor(float x, float y, Color &col);

    bool    _hasImage;
    QImage  _image;
    float   _scaleFactor;
};

#endif // IMAGETEXTURE_H
