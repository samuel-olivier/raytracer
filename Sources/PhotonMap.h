#ifndef PHOTONMAP_H
#define PHOTONMAP_H

#include <QVector>

#include "Ray.h"
#include "Color.h"

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
    void    computeColor(QVector3D const& from, Color& color, int maxNumber, float maxDist);

private:
    bool    _throwPhoton(Ray const& ray, Color const& color, QVector<Photon*>& photons);

    Scene*      _scene;
    Type        _type;
    KDTreeNode* _tree;
};

#endif // PHOTONMAP_H
