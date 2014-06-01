#ifndef PHOTON_H
#define PHOTON_H

#include <QVector3D>

#include "Color.h"

class Photon
{
public:
    Photon(QVector3D const& position = QVector3D(), QVector3D const& direction = QVector3D(), Color const& power = Color::BLACK);
    virtual ~Photon();

    QVector3D   position;
    QVector3D   direction;
    Color       power;
};

#endif // PHOTON_H
