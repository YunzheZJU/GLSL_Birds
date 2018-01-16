#ifndef VBOCUBE_H
#define VBOCUBE_H

#include "global.h"

class VBOCube
{

private:
    unsigned int vaoHandle;

public:
    VBOCube();

    void render();
};

#endif // VBOCUBE_H
