#ifndef SPHERESKY_H
#define SPHERESKY_H

#include "Sky.h"

class ImageTexture;

class SphereSky : public Sky
{
public:
    SphereSky(QString const& path);
    SphereSky(ImageTexture* texture = 0);
    virtual ~SphereSky();

    ImageTexture*   texture() const;
    void            setTexture(ImageTexture* texture);
    void            setTexture(QString const& path);

    virtual void    evaluate(QVector3D const& direction, Color& color);

private:
    ImageTexture* _texture;
};

#endif // SPHERESKY_H
