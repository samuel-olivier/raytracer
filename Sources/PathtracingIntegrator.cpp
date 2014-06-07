#include "PathtracingIntegrator.h"

#include "Scene.h"
#include "Material.h"
#include "Sky.h"
#include "Light.h"
#include "Config.h"

PathtracingIntegrator::PathtracingIntegrator()
    : _scene(0)
{
}

PathtracingIntegrator::~PathtracingIntegrator()
{
}

void PathtracingIntegrator::preprocess(Scene *scene, Camera *camera)
{
    _scene = scene;
    Integrator::preprocess(scene, camera);
}

void PathtracingIntegrator::compute(const Ray &ray, Intersection &hit)
{
    hit.shade = Color::BLACK;
    if (!_scene) {
        return ;
    }
    if (_scene->intersect(ray, hit)) {
        if (hit.light != 0) {
            if (ray.type != Ray::Diffused) {
                hit.light->intersectionColor(hit.shade);
            }
            return ;
        }
        if (hit.material == 0) {
            return ;
        }
        if (QVector3D::dotProduct(hit.normal, ray.direction) > 0) {
            hit.normal *= -1;
            hit.u *= -1;
            hit.v *= -1;
        }
        hit.material->applyTransformation(hit);
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
                        Color materialColor = Color::BLACK;
                        hit.material->computeReflectance(materialColor, toLight, ray, hit);
                        materialColor.Scale(cosTheta);
                        color.Multiply(materialColor);
                        hit.shade.AddScaled(color, bright);
                    }
                }
            }
        }
        if (ray.depth < config->pathtracingPathDepth()) {
            Ray newRay;
            Color c;

            newRay.type = Ray::Reflected;
            if (hit.material->sampleRay(ray, hit, newRay, c)) {
                newRay.depth = ray.depth + 1;
                newRay.time = ray.time;
                Intersection newHit;
                compute(newRay, newHit);
                newHit.shade.Multiply(c);
                hit.shade.Add(newHit.shade);
            }
        }
    } else {
        _scene->sky()->evaluate(ray.direction, hit.shade);
    }}

Integrator::Type PathtracingIntegrator::type() const
{
    return Pathtracing;
}

bool PathtracingIntegrator::_isShaded(const QVector3D &hitPosition, const QVector3D &toLight, const QVector3D &lightPos, float time)
{
    Intersection hit;
    Ray shadowRay(hitPosition, toLight, Ray::Shadow);

    shadowRay.time = time;
    hit.hitDistance = hitPosition.distanceToPoint(lightPos);
    return _scene->intersect(shadowRay, hit);
}
