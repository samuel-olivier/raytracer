#ifndef IMAGETEXTURE_H
#define IMAGETEXTURE_H

#include "Texture.h"

#include <QString>
#include <QImage>
#include <QColor>

class ImageTexture : public Texture
{
public:
    ImageTexture(QString const& path = QString());
    virtual ~ImageTexture();

    bool            hasImage() const;

    const QImage&   image() const;
    void            setImage(const QImage &image);
    bool            loadImage(QString const& filename);

    virtual void    evaluateColor(float x, float y, Color& value);
    virtual void    evaluateFloat(float x, float y, float& value);

private:
    bool    _hasImage;
    QImage  _image;
};

#endif // IMAGETEXTURE_H
