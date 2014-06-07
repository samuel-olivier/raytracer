#include "ImageTexture.h"

#include <QDebug>

#include "Logger.h"

ImageTexture::ImageTexture(const QString &path, float scaleFactor)
{
    loadImage(path);
    setScaleFactor(scaleFactor);
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
    _pixelColor(x, y, value);
}

void ImageTexture::evaluateFloat(float x, float y, float &value)
{
    Color col;
    _pixelColor(x, y, col);
    value = _scaleFactor * (col.Red + col.Green + col.Blue) / 3.f;
}

float ImageTexture::scaleFactor() const
{
    return _scaleFactor;
}

void ImageTexture::setScaleFactor(float scaleFactor)
{
    _scaleFactor = scaleFactor;
}

void ImageTexture::_pixelColor(float x, float y, Color &col)
{
    if (_hasImage) {
        y *= -1;
        wrapCoordinates(x, y);
        int xCoord = (x <= 0) ? 0 : ((x >= 1.0f) ? (_image.width() - 1) : int(x * _image.width()));
        int yCoord = (y <= 0) ? 0 : ((y >= 1.0f) ? (_image.height() - 1) : int(y * _image.height()));
        xCoord = qMax(0, qMin(_image.width() - 1, xCoord));
        yCoord = qMax(0, qMin(_image.height() - 1, yCoord));
        QColor color = _image.pixel(xCoord, yCoord);
        col.Set(color.redF(), color.greenF(), color.blueF());
    } else {
        col = Color::BLACK;
    }
}

