#include "PhotonMap.h"

#include <queue>
#include <QtMath>

#include "Scene.h"
#include "Light.h"
#include "KDTreeNode.h"
#include "Config.h"
#include "Photon.h"
#include "Material.h"

PhotonMap::PhotonMap(Scene* scene)
    : _scene(scene)
{
}

PhotonMap::~PhotonMap()
{
}

void PhotonMap::build(PhotonMap::Type type)
{
    _type = type;
    QVector<Photon*> photons;
    for (Light* light : _scene->lights()) {
        QVector<Photon*> lightPhotons;
        lightPhotons.reserve(config->photonNumber());
        int thrownPhotonNumber = 0;
        while (lightPhotons.size() < config->photonNumber()) {
            float intensity;
            Color color;
            Ray photonRay;
            photonRay.type = Ray::Photon;
            light->sampleRay(photonRay, intensity, color);
            color.Scale(intensity);
            _throwPhoton(photonRay, color, lightPhotons);
            ++thrownPhotonNumber;
            if (thrownPhotonNumber > config->photonNumber() * 2 && lightPhotons.size() == 0) {
                break ;
            }
        }
        if (lightPhotons.size() > 0) {
            float scale = 1.0f / thrownPhotonNumber;
            for (Photon* current : lightPhotons) {
                current->power.Scale(scale);
            }
            photons += lightPhotons;
        }
    }
    qDebug() << photons.size();
    if (photons.size() > 0) {
        _tree = new KDTreeNode;
        _tree->construct(photons);
    }
}

void PhotonMap::computeColor(const QVector3D &from, Color &color, int maxNumber, float maxDist)
{
    std::priority_queue<KDTreeNode::SearchItem> nearestPhotons;
    _tree->findNearest(nearestPhotons, from, maxNumber, maxDist);
    if (nearestPhotons.size() > 0) {
        float areaCoeff = 1.0f / (maxDist * M_PI);
        while (nearestPhotons.size() > 0) {
            color.Add(nearestPhotons.top().photon->power);
            nearestPhotons.pop();
        }
        color.Scale(areaCoeff);
    }
}

bool PhotonMap::_throwPhoton(const Ray &ray, const Color &color, QVector<Photon *> &photons)
{
    Intersection hit;
    if (photons.size() < config->photonNumber() && _scene->intersect(ray, hit) && hit.material) {
        if (_type == Type::Global) {
            bool res = false;
            if (hit.material->isType(Material::Diffuse)) {
                Photon* photon = new Photon(hit.position, ray.direction, color);
                photons << photon;
                res = true;
            }
            if (ray.depth < 5) {
                Ray newRay;
                Color c;

                newRay.type = Ray::Photon;
                if (hit.material->sampleRay(ray, hit, newRay, c)) {
                    newRay.depth = ray.depth + 1;
                    c.Multiply(color);
                    if (_throwPhoton(newRay, c, photons)) {
                        res = true;
                    }
                }
            }
            return res;
        } else if (_type == Type::Caustic) {
            if (hit.material->isType(Material::Diffuse) && ray.depth > 0) {
                Photon* photon = new Photon(hit.position, ray.direction, color);
                photons << photon;
                return true;
            }
            if (ray.depth < 5 && (hit.material->isType(Material::Reflection) || hit.material->isType(Material::Transmission))) {
                Ray newRay;
                Color c;

                newRay.type = Ray::Photon;
                if (hit.material->sampleRay(ray, hit, newRay, c)) {
                    newRay.depth = ray.depth + 1;
                    c.Multiply(color);
                    return _throwPhoton(newRay, c, photons);
                }
            }
        }
    }
    return false;
}
