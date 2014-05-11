#include "ImageTexture.h"

#include <QDebug>

#include "Logger.h"

ImageTexture::ImageTexture(const QString &path)
{
    loadImage(path);
}

ImageTexture::~ImageTexture()
{
}

bool ImageTexture::hasImage() const
{
    return _hasImage;
}

const QImage &ImageTexture::image() const
{
    return _image;
}

void ImageTexture::setImage(const QImage &image)
{
    _image = image;
    _hasImage = !_image.isNull();
}

bool ImageTexture::loadImage(const QString &filename)
{
    _hasImage = _image.load(filename);
    if (_hasImage == false) {
        logger->writeError(QString("Could not load texture \"%1\"").arg(filename));
    }
    return _hasImage;
}

void ImageTexture::evaluateColor(float x, float y, Color &value)
{
    if (_hasImage) {
        wrapCoordinates(x, y);
        y = 1.0f - y;
        int xCoord = (x < 0) ? 0 : ((x >= 1.0f) ? (_image.width() - 1) : int(x * _image.width()));
        int yCoord = (y < 0) ? 1 : ((y >= 1.0f) ? (_image.height() - 1) : int(y * _image.height()));
        QColor color = _image.pixel(xCoord, yCoord);
        value.Set(color.redF(), color.greenF(), color.blueF());
    } else {
        Texture::evaluateColor(x, y, value);
    }
}

void ImageTexture::evaluateFloat(float x, float y, float &value)
{
    if (_hasImage) {
        y *= -1;
        wrapCoordinates(x, y);
        int xCoord = (x <= 0) ? 0 : ((x >= 1.0f) ? (_image.width() - 1) : int(x * _image.width()));
        int yCoord = (y <= 0) ? 0 : ((y >= 1.0f) ? (_image.height() - 1) : int(y * _image.height()));
//        int yCoord = (y < 0) ? 1 : ((y >= 1.0f) ? (_image.height() - 1) : int(y * _image.height()));
        QColor color = _image.pixel(xCoord, yCoord);
        Color c;
        c.Set(color.redF(), color.greenF(), color.blueF());
        value = float(c.ToInt());
    } else {
        Texture::evaluateFloat(x, y, value);
    }}

