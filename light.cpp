//
// Light.cpp
// Functions related with lighting are defined here
// Created by Yunzhe on 2017/12/4.
//

#include "light.h"

// Colors to be used
GLfloat gray[4] = {0.4f, 0.4f, 0.4f, 1.0f};
GLfloat white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat black[4] = {0.0f, 0.0f, 0.0f, 1.0f};

GLfloat lightPosition0[4] = {0.0f, 0.0f, 10.0f, 1.0f};
GLfloat currentColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat constantattenuation = 1.0f;

void SetUpLights() {
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    // Set global ambient light
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gray);
    // Set mirror reflection target to camera instead of infinity
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    // Apply mirror reflection after texturing
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SEPARATE_SPECULAR_COLOR_EXT);
    // Set mix mode for texture and lighting
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, white);
    glEnable(GL_LIGHT0);
}
