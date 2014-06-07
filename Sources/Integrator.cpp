#include "Integrator.h"

#include "PathtracingIntegrator.h"
#include "PhotonMappingIntegrator.h"

Integrator::Integrator()
    : _isPreprocessed(false)
{

}

Integrator::~Integrator()
{

}

void Integrator::preprocess(Scene *, Camera *)
{
    _isPreprocessed = true;
}

bool Integrator::isPreprocessed() const
{
    return _isPreprocessed;
}

Integrator *Integrator::instanciateIntegrator(Integrator::Type type)
{
    switch (type) {
    case Pathtracing:
        return new PathtracingIntegrator;
    case PhotonMapping:
        return new PhotonMappingIntegrator;
    default:
        return 0;
    }
}
