//
// Created by Yunzhe on 2017/12/4.
//

#ifndef GPUBASEDRENDERING_PROJECT_GLOBAL_H
#define GPUBASEDRENDERING_PROJECT_GLOBAL_H

// Include system head files
#include <cmath>
#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <ctime>

// Include glew and glut
#include <glew.h>
#include <glut.h>

// Include glm
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;
#include <glm/gtc/matrix_transform.hpp>

// Define shortcuts for coordinate in location array
#define X 0
#define Y 1
#define Z 2

// Define abbreviate
#define R 0		// Radius
#define A 1		// Alpha
#define T 2		// Theta
#define W 0		// Width
#define H 1		// Height

// Define Pi in math
#define PI 3.1415926

// Define rotating pace in observing mode
#define OBSERVING_PACE 0.5

// Define moving pace in fps mode
#define MOVING_PACE 20

// Define paning pace in fps mode: Should be 1
#define PANNING_PACE 1

// Define the size of the area: width = BOUNDS, height = BOUNDS, center = (0, 0)
#define BOUNDS 400

// Define default camera position
#define DEFAULT_CAMERA_X 0
#define DEFAULT_CAMERA_Y 0
#define DEFAULT_CAMERA_Z 350

// Define default target position
#define DEFAULT_TARGET_X 0
#define DEFAULT_TARGET_Y 0
#define DEFAULT_TARGET_Z 0

// Define default camera polar
#define DEFAULT_POLAR_R 350
#define DEFAULT_POLAR_A 0
#define DEFAULT_POLAR_T 0

// Define speed of parameter control
#define PARA_SPEED 2.5

#endif //GPUBASEDRENDERING_PROJECT_GLOBAL_H
