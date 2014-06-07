#ifndef PHOTONMAPPINGINTEGRATOR_H
#define PHOTONMAPPINGINTEGRATOR_H

#include "Integrator.h"

class PhotonMap;

class PhotonMappingIntegrator : public Integrator
{
public:
    PhotonMappingIntegrator();
    virtual ~PhotonMappingIntegrator();

    virtual void    preprocess(Scene* scene, Camera* camera);
    virtual void    compute(Ray const& ray, Intersection &hit);
    virtual Type    type() const;

private:
    void    _raytrace(Ray const& ray, Intersection &hit, bool searchDiffuse);
    bool    _isShaded(QVector3D const& hitPosition, QVector3D const& toLight, QVector3D const& lightPos, float time);

    Scene*      _scene;
    PhotonMap*  _globalPhotonMap;
    PhotonMap*  _causticPhotonMap;
};

#endif // PHOTONMAPPINGINTEGRATOR_H
