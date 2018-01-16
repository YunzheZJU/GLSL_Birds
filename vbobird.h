//
// Created by Yunzhe on 2018/1/16.
//

#ifndef GPUBASEDRENDERING_PROJECT_VBOBIRD_H
#define GPUBASEDRENDERING_PROJECT_VBOBIRD_H

#include "global.h"
#include "drawable.h"

class VBOBird : public Drawable {
private:
    unsigned int vaoHandle;
    int faces;

    void generateVerts();
public:
    VBOBird(int number);

    void render() const;
};


#endif //GPUBASEDRENDERING_PROJECT_VBOBIRD_H
