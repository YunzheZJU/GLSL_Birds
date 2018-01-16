//
// Created by Yunzhe on 2017/12/4.
//

#ifndef GPUBASEDRENDERING_A1_DRAW_H
#define GPUBASEDRENDERING_A1_DRAW_H

// include global.h
#include "global.h"

extern GLuint vaoHandle;

void DrawScene();
void drawLocator(GLfloat* center, GLfloat radius);
void drawCrosshair();

#endif //GPUBASEDRENDERING_A1_DRAW_H
