//
// Created by Yunzhe on 2017/12/4.
//

#ifndef GPUBASEDRENDERING_PROJECT_SYSTEM_H
#define GPUBASEDRENDERING_PROJECT_SYSTEM_H

// Include related head files
#include "global.h"
#include "utility.h"
#include "Shader.h"
#include "vbobird.h"

// Using namespace std for cout
using namespace std;

extern Shader birdShader;
extern Shader computeShader;
extern int base;

void Idle();

void Redraw();

void Reshape(int width, int height);

void ProcessMouseMoving(int x, int y);

void ProcessFocus(int state);

void ProcessNormalKey(unsigned char k, int x, int y);

void ProcessSpecialKey(int k, int x, int y);

void PrintStatus();

void setupVBO();

void setupShader();

void updateBirdShaderUniform();

void updateComputeShaderUniform();

void setupTexture();

void setupFBO();

void setupVAO();

void init();

#endif //GPUBASEDRENDERING_PROJECT_SYSTEM_H
