#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

class PointLight : public Light
{
public:
    PointLight();
    virtual ~PointLight();

    QVector3D const&    position() const;
    void                setPosition(QVector3D const& position);

    virtual float       illuminate(const QVector3D &pos, Color &col, QVector3D &toLight, QVector3D &ltPos);

    virtual int         sampleNumber() const;

private:
    QVector3D   _position;
};

#endif // POINTLIGHT_H
