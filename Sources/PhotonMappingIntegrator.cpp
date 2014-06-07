#include "PhotonMappingIntegrator.h"

#include <QTime>

#include "PhotonMap.h"
#include "Logger.h"
#include "Scene.h"
#include "Material.h"
#include "Sky.h"
#include "Light.h"
#include "Config.h"

PhotonMappingIntegrator::PhotonMappingIntegrator()
    : _scene(0)
{
}

PhotonMappingIntegrator::~PhotonMappingIntegrator()
{

}

void PhotonMappingIntegrator::preprocess(Scene *scene, Camera *camera)
{
    _scene = scene;
    if (isPreprocessed() == false) {
        QTime time;
        time.restart();
        logger->showMessage("Building Global Photon Map...");
        _globalPhotonMap = new PhotonMap(_scene);
        _globalPhotonMap->build(PhotonMap::Global);
        logger->writeSuccess(QString("Global Photon Map built : %1 ms").arg(QString::number(time.elapsed())));
        time.restart();
        logger->showMessage("Building Caustic Photon Map...");
        _causticPhotonMap = new PhotonMap(_scene);
        _causticPhotonMap->build(PhotonMap::Caustic);
        logger->clearMessage();
        logger->writeSuccess(QString("Global Caustic Map built : %1 ms").arg(QString::number(time.elapsed())));
    }
    Integrator::preprocess(scene, camera);
}

void PhotonMappingIntegrator::compute(const Ray &ray, Intersection &hit)
{
    _raytrace(ray, hit, false);
}

Integrator::Type PhotonMappingIntegrator::type() const
{
    return PhotonMapping;
}

void PhotonMappingIntegrator::_raytrace(const Ray &ray, Intersection &hit, bool searchDiffuse)
{
    hit.shade = Color::BLACK;
    if (!_scene) {
        return ;
    }
    if (_scene->intersect(ray, hit)) {
        if (hit.light != 0 && searchDiffuse == false) {
            hit.light->intersectionColor(hit.shade);
            return ;
        }
        if (!hit.material) {
            return ;
        }
        if (QVector3D::dotProduct(hit.normal, ray.direction) > 0) {
            hit.normal *= -1;
            hit.u *= -1;
            hit.v *= -1;
        }
        hit.material->applyTransformation(hit);

        Ray newRay;
        Color c;

        bool sample = hit.material->sampleRay(ray, hit, newRay, c);

        if (ray.depth < config->photonMappingRayDepth()) {
            if (sample && (searchDiffuse == false || newRay.type != Ray::Diffused)) {
                newRay.depth = ray.depth + 1;
                Intersection newHit;
                newRay.time = ray.time;
                _raytrace(newRay, newHit, searchDiffuse || newRay.type == Ray::Diffused);
                newHit.shade.Multiply(c);
                hit.shade.Add(newHit.shade);
            }
        }
        if (searchDiffuse == false) {
            for (Light* light : _scene->lights()) {
                int sampleNumber = light->sampleNumber();
                for (int i = 0; i < sampleNumber; ++i) {
                    Color color;
                    QVector3D toLight;
                    QVector3D lightPos;
                    float bright = light->illuminate(hit.position, color, toLight, lightPos) / sampleNumber;
                    float cosTheta = QVector3D::dotProduct(toLight, hit.normal);
                    if (cosTheta >= Config::Epsilon && bright >= Config::Epsilon) {
                        if (!config->shadowEnabled() || !_isShaded(hit.position, toLight, lightPos, ray.time)) {
                            Color materialColor = Color::WHITE;
                            hit.material->computeReflectance(materialColor, toLight, ray, hit);
                            materialColor.Scale(cosTheta);
                            color.Multiply(materialColor);
                            hit.shade.AddScaled(color, bright);
                        }
                    }
                }
            }
            if (_causticPhotonMap && newRay.type == Ray::Diffused) {
                Color causticColor;
                _causticPhotonMap->computeColor(ray, hit, causticColor, config->photonMappingNumberNearestPhoton(), config->photonMappingMaximumSearchRadius());
                hit.shade.Add(causticColor);
            }
        } else if (newRay.type == Ray::Diffused && _globalPhotonMap) {
            Color globalColor;
            _globalPhotonMap->computeColor(ray, hit, globalColor, config->photonMappingNumberNearestPhoton(), config->photonMappingMaximumSearchRadius());
            hit.shade.Add(globalColor);
        }

//        if (_globalPhotonMap) {
//            Color globalColor;
//            _globalPhotonMap->computeColor(ray, hit, globalColor, config->numberNearestPhoton(), config->photonMaximumRadius());
//            hit.shade.Add(globalColor);
//        }
//        if (_causticPhotonMap) {
//            Color causticColor;
//            _causticPhotonMap->computeColor(ray, hit, causticColor, config->numberNearestPhoton(), config->photonMaximumRadius());
//            hit.shade.Add(causticColor);
//        }
    } else {
        _scene->sky()->evaluate(ray.direction, hit.shade);
    }}

bool PhotonMappingIntegrator::_isShaded(const QVector3D &hitPosition, const QVector3D &toLight, const QVector3D &lightPos, float time)
{
    Intersection hit;
    Ray shadowRay(hitPosition, toLight, Ray::Shadow);

    shadowRay.time = time;
    hit.hitDistance = hitPosition.distanceToPoint(lightPos);
    return _scene->intersect(shadowRay, hit);
}
