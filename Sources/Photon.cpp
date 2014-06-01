#include "Photon.h"

Photon::Photon(const QVector3D &position, const QVector3D &direction, const Color &power)
    : position(position), direction(direction), power(power)
{
}

Photon::~Photon()
{

}
