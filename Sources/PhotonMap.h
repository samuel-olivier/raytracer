#ifndef PHOTONMAP_H
#define PHOTONMAP_H

#include <QVector>
#include <QMutex>

#include "Ray.h"
#include "Color.h"
#include "Intersection.h"
#include "Ray.h"

class KDTreeNode;
class Photon;
class Scene;

class PhotonMap
{
public:

    enum Type {
        Global,
        Caustic
    };

    PhotonMap(Scene *scene);
    virtual ~PhotonMap();

    void    build(Type type);
    void    computeColor(Ray const& ray, Intersection const& hit, Color& color, int maxNumber, float maxDist);

private:
    struct ThreadParam
    {
        Light*              light;
        int                 thrownPhotonNumber;
        QMutex              photonsMutex;
        QVector<Photon*>    photons;
        int                 maxPhotons;
    };

    void    _buildThread();
    bool    _throwPhoton(Ray const& ray, Color const& color, QVector<Photon*>& photons, int maxPhotons);

    Scene*      _scene;
    Type        _type;
    KDTreeNode* _tree;
    ThreadParam* _currentThread;
};

#endif // PHOTONMAP_H
