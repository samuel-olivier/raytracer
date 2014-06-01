#include "KDTreeNode.h"

#include <QDebug>

#include "Photon.h"
#include "BoundingBox.h"

class PhotonComp {
public:
    PhotonComp(int dim)
        : _dim(dim)
    {
    }

    bool    operator()(Photon* p1, Photon* p2)
    {
        if (!p1 || !p2) {
            false;
        }
        return p1->position[_dim] < p2->position[_dim];
    }

private:
    int _dim;
};


KDTreeNode::KDTreeNode()
    : _photon(0), _axis(0), _left(0), _right(0)
{
}

KDTreeNode::~KDTreeNode()
{
}

void KDTreeNode::construct(const QVector<Photon *> &photons)
{
    Photon** array = new Photon*[photons.size()];
    for (int i = 0; i < photons.size(); ++i) {
        array[i] = photons[i];
    }
    _construct(array, 0, photons.size());
    delete []array;
}

void KDTreeNode::findNearest(std::priority_queue<SearchItem> &items, const QVector3D &from, int maxNumber, float& maxDist)
{
    if (_photon) {
        QVector3D toPhoton = from - _photon->position;
        float distPlane = toPhoton[_axis];

        if (distPlane < 0) {
            if (_left) {
                _left->findNearest(items, from, maxNumber, maxDist);
            }
            if (_right && distPlane * distPlane < maxDist) {
                _right->findNearest(items, from, maxNumber, maxDist);
            }
        } else {
            if (_right) {
                _right->findNearest(items, from, maxNumber, maxDist);
            }
            if (_left && distPlane * distPlane < maxDist) {
                _left->findNearest(items, from, maxNumber, maxDist);
            }
        }
        float distSquared = toPhoton.lengthSquared();

        if (distSquared < maxDist) {
            SearchItem resItem;
            resItem.distSquared = distSquared;
            resItem.photon = _photon;

            items.push(resItem);
            if (items.size() > maxNumber) {
                items.pop();
            }
            if (items.size() == maxNumber) {
                maxDist = items.top().distSquared;
            }
        }
    }
}

void KDTreeNode::_construct(Photon **photons, int begin, int end)
{
    int size = end - begin;
    if (size == 1) {
        _photon = photons[begin];
    } else if (size > 1) {
        BoundingBox b;
        for (int i = begin; i < end; ++i) {
            b.merge(photons[i]->position);
        }

        QVector3D bSize = b.size();
        _axis = 2;
        if (bSize.x() > bSize.y() && bSize.x() > bSize.z()) {
            _axis = 0;
        } else if (bSize.y() > bSize.x() && bSize.y() > bSize.z()) {
            _axis = 1;
        }
        PhotonComp comp(_axis);
        int mid = (end + begin) / 2;
        std::nth_element(&photons[begin], &photons[mid], &photons[end], comp);
        _photon = photons[mid];
        if (mid - begin > 0) {
            _left = new KDTreeNode;
            _left->_construct(photons, begin, mid);
        }
        if (end - mid > 0) {
            _right = new KDTreeNode;
            _right->_construct(photons, mid + 1, end);
        }
    }
}


bool KDTreeNode::SearchItem::operator<(const KDTreeNode::SearchItem &other) const
{
    return this->distSquared < other.distSquared;
}
