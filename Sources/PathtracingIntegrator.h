#ifndef PATHTRACINGINTEGRATOR_H
#define PATHTRACINGINTEGRATOR_H

#include "Integrator.h"

class PathtracingIntegrator : public Integrator
{
public:
    PathtracingIntegrator();
    virtual ~PathtracingIntegrator();

    virtual void    preprocess(Scene* scene, Camera* camera);
    virtual void    compute(Ray const& ray, Intersection &hit);
    virtual Type    type() const;

private:
    bool    _isShaded(QVector3D const& hitPosition, QVector3D const& toLight, QVector3D const& lightPos, float time);

    Scene*  _scene;
};

#endif // PATHTRACINGINTEGRATOR_H
