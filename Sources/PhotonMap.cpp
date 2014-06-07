#include "PhotonMap.h"

#include <queue>
#include <QtMath>
#include <thread>

#include "Scene.h"
#include "Light.h"
#include "KDTreeNode.h"
#include "Config.h"
#include "Photon.h"
#include "Material.h"

PhotonMap::PhotonMap(Scene* scene)
    : _scene(scene), _tree(0)
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
        if (light->generatePhotons()) {
            _currentThread = new ThreadParam;
            _currentThread->light = light;
            _currentThread->maxPhotons = config->photonMappingPhotonNumber() / config->threadNumber();
            _currentThread->thrownPhotonNumber = 0;
            QList<std::thread*> computingThreads;
            for (int i = 0; i < config->threadNumber(); ++i) {
                computingThreads.append(new std::thread(&PhotonMap::_buildThread, this));
            }
            for (std::thread* computingThread : computingThreads) {
                computingThread->join();
                delete computingThread;
            }
            if (_currentThread->photons.size() > 0) {
                float scale = 1.0f / _currentThread->thrownPhotonNumber;
                for (Photon* current : _currentThread->photons) {
                    current->power.Scale(scale);
                }
                photons += _currentThread->photons;
            }
        }
    }
    if (photons.size() > 0) {
        _tree = new KDTreeNode;
        _tree->construct(photons);
    }
}

//void PhotonMap::build(PhotonMap::Type type)
//{
//    _type = type;
//    QVector<Photon*> photons;
//    for (Light* light : _scene->lights()) {
//        QVector<Photon*> lightPhotons;
//        lightPhotons.reserve(config->photonNumber());
//        int thrownPhotonNumber = 0;
//        while (lightPhotons.size() < config->photonNumber()) {
//            float intensity;
//            Color color;
//            Ray photonRay;
//            light->sampleRay(photonRay, intensity, color);
//            color.Scale(intensity);
//            _throwPhoton(photonRay, color, lightPhotons);
//            ++thrownPhotonNumber;
//            if (thrownPhotonNumber > config->photonNumber() * 2 && lightPhotons.size() == 0) {
//                break ;
//            }
//        }
//        if (lightPhotons.size() > 0) {
//            float scale = 1.0f / thrownPhotonNumber;
//            for (Photon* current : lightPhotons) {
//                current->power.Scale(scale);
//            }
//            photons += lightPhotons;
//        }
//    }
//    if (photons.size() > 0) {
//        _tree = new KDTreeNode;
//        _tree->construct(photons);
//    }
//}

void PhotonMap::computeColor(const Ray &ray, const Intersection &hit, Color &color, int maxNumber, float maxDist)
{
    color = Color::BLACK;
    if (_tree == 0) {
        return ;
    }
    std::priority_queue<KDTreeNode::SearchItem> nearestPhotons;
    _tree->findNearest(nearestPhotons, hit.position, maxNumber, maxDist);
    if (nearestPhotons.size() > 0) {
        float areaCoeff = 1.0f / (M_PI * maxDist);
        while (nearestPhotons.size() > 0) {
            KDTreeNode::SearchItem const& item = nearestPhotons.top();
            nearestPhotons.pop();

            float cosTheta = QVector3D::dotProduct(item.photon->direction, hit.normal);
            if (cosTheta >= Config::Epsilon) {
                Color c;
                hit.material->computeReflectance(c, item.photon->direction, ray, hit);
                c.Multiply(item.photon->power);
                color.Add(c);
                //            color.AddScaled(c, cosTheta);
                //            color.Add(item.photon->power);
            }
        }
        color.Scale(areaCoeff);

    }
}

void PhotonMap::_buildThread()
{
    int thrownPhotonNumber = 0;
    QVector<Photon*> myPhotons;
    myPhotons.reserve(_currentThread->maxPhotons);
    while (myPhotons.size() < _currentThread->maxPhotons) {
        float intensity;
        Color color;
        Ray photonRay;
        _currentThread->light->sampleRay(photonRay, intensity, color);
        color.Scale(intensity);
        _throwPhoton(photonRay, color, myPhotons, _currentThread->maxPhotons);
        ++thrownPhotonNumber;
        if (thrownPhotonNumber > _currentThread->maxPhotons && myPhotons.size() == 0) {
            break ;
        }
    }
    _currentThread->photonsMutex.lock();
    _currentThread->photons += myPhotons;
    _currentThread->thrownPhotonNumber += thrownPhotonNumber;
    _currentThread->photonsMutex.unlock();
}

int plop = 0;

bool PhotonMap::_throwPhoton(const Ray &ray, const Color &color, QVector<Photon *> &photons, int maxPhotons)
{
    if (ray.depth > config->photonMappingMapDepth()) {
        return false;
    }
    Intersection hit;
    if (photons.size() < maxPhotons && _scene->intersect(ray, hit)) {
        if (QVector3D::dotProduct(hit.normal, ray.direction) > 0) {
            hit.normal *= -1;
            hit.u *= -1;
            hit.v *= -1;
        }
        if (hit.material) {
            hit.material->applyTransformation(hit);
        }
        if (_type == Type::Global) {
            bool res = false;
            Ray newRay;
            Color c;

            bool sample = hit.material->sampleRay(ray, hit, newRay, c);
            if (newRay.type == Ray::Diffused) {
                Photon* photon = new Photon(hit.position, -ray.direction, color);
                photons << photon;
                res = true;
            }
            if (sample) {
                newRay.depth = ray.depth + 1;
                c.Multiply(color);
                if (_throwPhoton(newRay, c, photons, maxPhotons)) {
                    res = true;
                }
            }
            return res;
        } else if (_type == Type::Caustic) {
            Ray newRay;
            Color c;

            bool sample = hit.material->sampleRay(ray, hit, newRay, c);
            if (newRay.type == Ray::Diffused && ray.depth > 0) {
                Photon* photon = new Photon(hit.position, -ray.direction, color);
                photons << photon;
                return true;
            } else if (sample && (newRay.type == Ray::Reflected || newRay.type == Ray::Transmitted)) {
                newRay.depth = ray.depth + 1;
                c.Multiply(color);
                return _throwPhoton(newRay, c, photons, maxPhotons);
            }
        }
    }
    return false;
}

//bool PhotonMap::_throwPhoton(const Ray &ray, const Color &color, QVector<Photon *> &photons)
//{
//    if (ray.depth >= 5) {
//        return false;
//    }
//    Intersection hit;
//    if (photons.size() < config->photonNumber() && _scene->intersect(ray, hit) && hit.material) {
//        if (QVector3D::dotProduct(hit.normal, ray.direction) > 0) {
//            hit.normal *= -1;
//            hit.u *= -1;
//            hit.v *= -1;
//        }
//        if (hit.material) {
//            hit.material->applyTransformation(hit);
//        }
//        if (_type == Type::Global) {
//            bool res = false;
//            Ray newRay;
//            Color c;

//            bool sample = hit.material->sampleRay(ray, hit, newRay, c);
//            if (newRay.type == Ray::Diffused) {
//                Photon* photon = new Photon(hit.position, -ray.direction, color);
//                photons << photon;
//                res = true;
//            }
//            if (sample) {
//                newRay.depth = ray.depth + 1;
//                c.Multiply(color);
//                if (_throwPhoton(newRay, c, photons)) {
//                    res = true;
//                }
//            }
//            return res;
//        } else if (_type == Type::Caustic) {
//            Ray newRay;
//            Color c;

//            bool sample = hit.material->sampleRay(ray, hit, newRay, c);
//            if (newRay.type == Ray::Diffused && ray.depth > 0) {
//                Photon* photon = new Photon(hit.position, -ray.direction, color);
//                photons << photon;
//                return true;
//            } else if (sample && (newRay.type == Ray::Reflected || newRay.type == Ray::Transmitted)) {
//                newRay.depth = ray.depth + 1;
//                c.Multiply(color);
//                return _throwPhoton(newRay, c, photons);
//            }
//        }
//    }
//    return false;
//}
