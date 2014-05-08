#ifndef BOX_H
#define BOX_H

#include "Mesh.h"

class Box : public Mesh
{
public:
    Box(Node* parent = 0);

    void set(float x, float y, float z);
};

#endif // BOX_H
