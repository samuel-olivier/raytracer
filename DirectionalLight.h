#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Light.h"

class DirectionalLight : public Light
{
public:
    DirectionalLight();
    virtual ~DirectionalLight();

    QVector3D const&    direction() const;
    void                setDirection(QVector3D const& direction);

    virtual float       illuminate(const QVector3D &pos, Color &col, QVector3D &toLight, QVector3D &ltPos);

    virtual int         sampleNumber() const;

private:
    QVector3D   _direction;
};

#endif // DIRECTIONALLIGHT_H
