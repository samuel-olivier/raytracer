#ifndef KDTREENODE_H
#define KDTREENODE_H

#include <QVector>
#include <QVector3D>
#include <queue>

class Photon;

class KDTreeNode
{
public:
    struct SearchItem {
        Photon* photon;
        float   distSquared;

        bool    operator<(SearchItem const& other) const;
    };

    KDTreeNode();
    virtual ~KDTreeNode();

    void    construct(QVector<Photon*> const& photons);
    void    findNearest(std::priority_queue<SearchItem>& items, QVector3D const& from, int maxNumber, float &maxDist);

private:
    void    _construct(Photon** photons, int begin, int end);

    Photon*     _photon;
    int         _axis;
    KDTreeNode* _left;
    KDTreeNode* _right;
};

#endif // KDTREENODE_H
