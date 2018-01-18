//
// Created by Yunzhe on 2017/12/4.
//

#ifndef GPUBASEDRENDERING_A1_SYSTEM_H
#define GPUBASEDRENDERING_A1_SYSTEM_H

// Include related head files
#include "global.h"
#include "utility.h"
#include "draw.h"
#include "light.h"
#include "textfile.h"
#include "Shader.h"
#include "vbotorus.h"
#include "vboteapot.h"
#include "vbomesh.h"
#include "vboplane.h"
#include "vbocube.h"
#include "vbobird.h"
#include "tgaio.h"

// Using namespace std for cout
using namespace std;

//extern GLuint vaoHandle;
extern Shader birdShader;
extern Shader computeShader;
//extern VBOTorus *torus;

void Idle();
void Redraw();
void Reshape(int width, int height);
void ProcessMouseClick(int button, int state, int x, int y);
void ProcessMouseMove(int x, int y);
void ProcessFocus(int state);
void ProcessNormalKey(unsigned char k, int x, int y);
void ProcessSpecialKey(int k, int x, int y);
void PrintStatus();
void SetBufferedObjects();
void initVBO();
void setShader();
void updateMVPZero();
void updateMVPOne();
void updateMVPTwo();
void updateMVPThree();
void updateShaderMVP();
void setupTexture();
void setupVAO();

#endif //GPUBASEDRENDERING_A1_SYSTEM_H
