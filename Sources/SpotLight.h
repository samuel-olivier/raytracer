#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "Light.h"

class SpotLight : public Light
{
public:
    SpotLight();
    virtual ~SpotLight();

    virtual float illuminate(const QVector3D& pos, Color& col, QVector3D& toLight, QVector3D& ltPos);
    virtual void  sampleRay(Ray& newRay, float& intensity, Color& color) const;
    virtual int sampleNumber() const;

    QVector3D const&    position() const;
    void                setPosition(QVector3D const& position);

    QVector3D const&    direction() const;
    void                setDirection(QVector3D const& direction);

    float   innerAngle() const;
    void    setInnerAngle(float innerAngle);

    float   outerAngle() const;
    void    setOuterAngle(float outerAngle);

private:
    float       _innerAngle;
    float       _outerAngle;
    QVector3D   _position;
    QVector3D   _direction;
};

#endif // SPOTLIGHT_H
