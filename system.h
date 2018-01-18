//
// Created by Yunzhe on 2017/12/4.
//

#ifndef GPUBASEDRENDERING_A1_SYSTEM_H
#define GPUBASEDRENDERING_A1_SYSTEM_H

// Include related head files
#include "global.h"
#include "utility.h"
#include "Shader.h"
#include "vbobird.h"

// Using namespace std for cout
using namespace std;

extern Shader birdShader;
extern Shader computeShader;

void Idle();

void Redraw();

void Reshape(int width, int height);

void ProcessMouseClick(int button, int state, int x, int y);

void ProcessMouseDrag(int x, int y);

void ProcessFocus(int state);

void ProcessNormalKey(unsigned char k, int x, int y);

void ProcessSpecialKey(int k, int x, int y);

void PrintStatus();

void initVBO();

void setShader();

void updateBirdShaderUniform();

void updateComputeShaderUniform();

void setupTexture();

void setupFBO();

void setupVAO();

#endif //GPUBASEDRENDERING_A1_SYSTEM_H
