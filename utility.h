//
// Created by Yunzhe on 2017/12/4.
//

#ifndef GPUBASEDRENDERING_A1_UTILITY_H
#define GPUBASEDRENDERING_A1_UTILITY_H

// include global.h
#include "global.h"

// Using namespace std for cout
using namespace std;

// Define BITMAP_ID
#define BITMAP_ID 0x4D42

void cameraMakeZero(GLfloat* camera, GLfloat* target, GLfloat* polar);
void updateCamera(GLfloat* camera, GLfloat* target, GLfloat* polar);
void updatePolar(GLfloat* camera, GLfloat* target, GLfloat* polar);
void updateTarget(GLfloat* camera, GLfloat* target, GLfloat* polar);
void saveCamera(GLfloat* camera, GLfloat* target, GLfloat* polar);
void loadCamera(GLfloat* camera, GLfloat* target, GLfloat* polar);
void updateWindowcenter(int* window, int* windowcenter);
bool screenshot(int width, int height);

#endif //GPUBASEDRENDERING_A1_UTILITY_H
