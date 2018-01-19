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

void ProcessMouseClick(int button, int state, int x, int y);

void ProcessMouseMoving(int x, int y);

void ProcessFocus(int state);

void ProcessNormalKey(unsigned char k, int x, int y);

void ProcessSpecialKey(int k, int x, int y);

void PrintStatus();

void initVBO();

void setupShader();

void updateBirdShaderUniform();

void updateComputeShaderUniform();

void setupTexture();

void setupFBO();

void setupVAO();

#endif //GPUBASEDRENDERING_PROJECT_SYSTEM_H
