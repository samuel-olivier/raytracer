#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "Ray.h"
#include "Intersection.h"

class Scene;
class Camera;

class Integrator
{
public:
    enum Type {
        Pathtracing,
        PhotonMapping
    };

    Integrator();
    virtual ~Integrator();

    virtual void    preprocess(Scene* scene, Camera* camera);
    bool            isPreprocessed() const;
    virtual void    compute(Ray const& ray, Intersection &hit) = 0;
    virtual Type    type() const = 0;

    static Integrator*  instanciateIntegrator(Type type);

private:
    bool    _isPreprocessed;
};

#endif // INTEGRATOR_H
