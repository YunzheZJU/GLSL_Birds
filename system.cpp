//
// System.cpp
// Processing system display and control
// Created by Yunzhe on 2017/12/4.
//

#include "system.h"
#include "glutils.h"

Shader birdShader = Shader();
Shader computeShader = Shader();
VBOBird *bird;
GLuint computeTexture;
GLuint coordTexture;
GLuint fboHandle;
GLuint fsQuad;
GLuint colorTypeBird[2];
GLuint positionGetterBird[2];
GLuint velocityGetterBird[2];
GLuint positionGetterCompute[2];
GLuint velocityGetterCompute[2];
GLuint positionSetterCompute[2];
GLuint velocitySetterCompute[2];
GLfloat camera[3] = {DEFAULT_CAMERA_X, DEFAULT_CAMERA_Y, DEFAULT_CAMERA_Z};         // Position of camera
GLfloat target[3] = {DEFAULT_TARGET_X, DEFAULT_TARGET_Y, DEFAULT_TARGET_Z};         // Position of target of camera
GLfloat polar[3] = {DEFAULT_POLAR_R, DEFAULT_POLAR_A, DEFAULT_POLAR_T};             // Polar coordinates of camera
bool bcamera = true;                               // Switch of camera/target control
bool bAnimation = true;
bool bRandomColor = true;
int fpsmode = 0;                                    // 0:off, 1:on, 2:waiting
int window[2] = {1280, 720};                        // Window size
int windowcenter[2];                                // Center of this window, to be updated
int base = 32;
int activeRegion = 0;
float mouse[2] = {1000.0f, 1000.0f};
float seperationDistance = 20.0f;
float alignmentDistance = 10.0f;
float cohesionDistance = 10.0f;
char message[70] = "Welcome!";                        // Message string to be shown

void Idle() {
    glutPostRedisplay();
}

void Reshape(int width, int height) {
    if (height == 0) {                        // Prevent A Divide By Zero By
        height = 1;                            // Making Height Equal One
    }
    glViewport(static_cast<GLint>(width / 2.0 - 640), static_cast<GLint>(height / 2.0 - 360), 1280, 720);
    window[W] = width;
    window[H] = height;
    updateWindowcenter(window, windowcenter);
}

void Redraw() {
    ///////////////Update texture//////////////
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glDisable(GL_DEPTH_TEST);
    computeShader.use();
    updateComputeShaderUniform();
    if (bAnimation) {
        glBindVertexArray(fsQuad);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        activeRegion = 1 - activeRegion;
    }
    computeShader.disable();
    glFlush();
    ///////////////////Draw the birds///////////////////
    birdShader.use();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    updateBirdShaderUniform();
    bird->render();
    birdShader.disable();
    //////////////////////End////////////////////////
    // Show fps, message and other information
    PrintStatus();
    glutSwapBuffers();

    GLUtils::checkForOpenGLError(__FILE__, __LINE__);
}

void ProcessMouseMoving(int x, int y) {
    mouse[X] = static_cast<float>((x - window[W] / 2) * 1.0 / (window[W] / 2) * 0.5);
    mouse[Y] = static_cast<float>((window[H] / 2 - y) * 1.0 / (window[H] / 2) * 0.5);
}

void ProcessFocus(int state) {
    if (state == GLUT_LEFT) {
        bAnimation = false;
        cout << "Focus is on other window." << endl;
        sprintf(message, "Focus is lost. Animation stops.");
    } else if (state == GLUT_ENTERED) {
        bAnimation = true;
        sprintf(message, "Welcome.");
    }
}

void ProcessNormalKey(unsigned char k, int x, int y) {
    switch (k) {
        // 退出程序
        case 27: {
            cout << "Bye." << endl;
            exit(0);
        }
            // 空格
        case 32: {
            cout << "Space pressed. Animation stops/continues playing.\n" << endl;
            strcpy(message, "Space pressed.  Animation stops/continues playing.");
            bAnimation = !bAnimation;
            break;
        }
            // 切换摄像机本体/焦点控制
        case 'Z':
        case 'z': {
            strcpy(message, "Z pressed. Switch camera control!");
            bcamera = !bcamera;
            break;
        }
            // 切换第一人称控制
        case 'C':
        case 'c': {
            strcpy(message, "C pressed. Switch fps control!");
            // 摄像机归零
            cameraMakeZero(camera, target, polar);
            if (!fpsmode) {
                // 调整窗口位置
                int windowmaxx = glutGet(GLUT_WINDOW_X) + window[W];
                int windowmaxy = glutGet(GLUT_WINDOW_Y) + window[H];
                if (windowmaxx >= glutGet(GLUT_SCREEN_WIDTH) || windowmaxy >= glutGet(GLUT_SCREEN_HEIGHT)) {
                    // glutPositionWindow()并不会立即执行！
                    glutPositionWindow(glutGet(GLUT_SCREEN_WIDTH) - window[W], glutGet(GLUT_SCREEN_HEIGHT) - window[H]);
                    fpsmode = 2;
                    break;
                }
                // 鼠标位置居中
                updateWindowcenter(window, windowcenter);
                // windowcenter[X] - window[W] * 0.25 为什么要减？
                SetCursorPos(windowcenter[X], windowcenter[Y]);
                glutSetCursor(GLUT_CURSOR_NONE);
                fpsmode = 1;
            } else {
                glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
                fpsmode = 0;
            }
            break;
        }
            // 第一人称移动/摄像机本体移动/焦点移动
        case 'A':
        case 'a': {
            strcpy(message, "A pressed. Watch carefully!");
            if (fpsmode) {
                saveCamera(camera, target, polar);
                camera[X] -= cos(polar[A]) * MOVING_PACE;
                camera[Z] += sin(polar[A]) * MOVING_PACE;
                target[X] -= cos(polar[A]) * MOVING_PACE;
                target[Z] += sin(polar[A]) * MOVING_PACE;
            } else {
                if (bcamera) {
                    polar[A] -= OBSERVING_PACE * 0.1;
                    updateCamera(camera, target, polar);
                    cout << fixed << setprecision(1) << "A pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                } else {
                    target[X] -= OBSERVING_PACE;
                    updatePolar(camera, target, polar);
                    cout << fixed << setprecision(1) << "A pressed.\n\tPosition of camera target is set to (" <<
                         target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
                }
            }
            break;
        }
        case 'D':
        case 'd': {
            strcpy(message, "D pressed. Watch carefully!");
            if (fpsmode) {
                saveCamera(camera, target, polar);
                camera[X] += cos(polar[A]) * MOVING_PACE;
                camera[Z] -= sin(polar[A]) * MOVING_PACE;
                target[X] += cos(polar[A]) * MOVING_PACE;
                target[Z] -= sin(polar[A]) * MOVING_PACE;
            } else {
                if (bcamera) {
                    polar[A] += OBSERVING_PACE * 0.1;
                    updateCamera(camera, target, polar);
                    cout << fixed << setprecision(1) << "D pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                } else {
                    target[X] += OBSERVING_PACE;
                    updatePolar(camera, target, polar);
                    cout << fixed << setprecision(1) << "D pressed.\n\tPosition of camera target is set to (" <<
                         target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
                }
            }
            break;
        }
        case 'W':
        case 'w': {
            strcpy(message, "W pressed. Watch carefully!");
            if (fpsmode) {
                saveCamera(camera, target, polar);
                camera[X] -= sin(polar[A]) * MOVING_PACE;
                camera[Z] -= cos(polar[A]) * MOVING_PACE;
                target[X] -= sin(polar[A]) * MOVING_PACE;
                target[Z] -= cos(polar[A]) * MOVING_PACE;
            } else {
                if (bcamera) {
                    camera[Y] += MOVING_PACE;
                    cout << fixed << setprecision(1) << "W pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                } else {
                    target[Y] += MOVING_PACE;
                    updatePolar(camera, target, polar);
                    cout << fixed << setprecision(1) << "W pressed.\n\tPosition of camera target is set to (" <<
                         target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
                }
            }
            break;
        }
        case 'S':
        case 's': {
            strcpy(message, "S pressed. Watch carefully!");
            if (fpsmode) {
                saveCamera(camera, target, polar);
                camera[X] += sin(polar[A]) * MOVING_PACE;
                camera[Z] += cos(polar[A]) * MOVING_PACE;
                target[X] += sin(polar[A]) * MOVING_PACE;
                target[Z] += cos(polar[A]) * MOVING_PACE;
            } else {
                if (bcamera) {
                    camera[Y] -= MOVING_PACE;
                    cout << fixed << setprecision(1) << "S pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                    strcpy(message, "S pressed. Watch carefully!");
                } else {
                    target[Y] -= MOVING_PACE;
                    updatePolar(camera, target, polar);
                    cout << fixed << setprecision(1) << "S pressed.\n\tPosition of camera target is set to (" <<
                         target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
                }
            }
            break;
        }
        case 'Q':
        case 'q': {
            if (!fpsmode) {
                if (bcamera) {
                    strcpy(message, "Q pressed. Camera is moved...nearer!");
                    polar[R] *= 0.95;
                    updateCamera(camera, target, polar);
                    cout << fixed << setprecision(1) << "Q pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                } else {
                    strcpy(message, "Q pressed. Camera target is moving towards +Z!");
                    target[Z] += MOVING_PACE;
                    updatePolar(camera, target, polar);
                    cout << fixed << setprecision(1) << "Q pressed.\n\tPosition of camera target is set to (" <<
                         target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
                }
            }
            break;
        }
        case 'E':
        case 'e': {
            if (!fpsmode) {
                if (bcamera) {
                    strcpy(message, "E pressed. Camera is moved...farther!");
                    polar[R] *= 1.05;
                    updateCamera(camera, target, polar);
                    cout << fixed << setprecision(1) << "E pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                } else {
                    strcpy(message, "E pressed. Camera target is moving towards -Z!");
                    target[Z] -= MOVING_PACE;
                    updatePolar(camera, target, polar);
                    cout << fixed << setprecision(1) << "E pressed.\n\tPosition of camera target is set to (" <<
                         target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
                }
            }
            break;
        }
            // 屏幕截图
        case 'X':
        case 'x': {
            cout << "X pressed." << endl;
            if (screenshot(window[W], window[H])) {
                cout << "Screenshot is saved." << endl;
                strcpy(message, "X pressed. Screenshot is Saved.");
            } else {
                cout << "Screenshot failed." << endl;
                strcpy(message, "X pressed. Screenshot failed.");
            }
            break;
        }
            // 颜色切换
        case 'R':
        case 'r': {
            bRandomColor = !bRandomColor;
            cout << "R pressed. Change color type." << endl;
            strcpy(message, "R pressed. Change color type.");
            break;
        }
        default:
            break;
    }
}

void ProcessSpecialKey(int k, int x, int y) {
    switch (k) {
        // Up arrow
        case GLUT_KEY_UP: {
            if (seperationDistance < 99.9f) {
                seperationDistance += PARA_SPEED;
            }
            cout << fixed << setprecision(1) << "Up arrow pressed. Seperation Distance is set to " << seperationDistance
                 << "." << endl;
            sprintf(message, "Up arrow pressed. Seperation Distance is set to %.1f.", seperationDistance);
            break;
        }
            // Down arrow
        case GLUT_KEY_DOWN: {
            if (seperationDistance > 0.1f) {
                seperationDistance -= PARA_SPEED;
            }
            cout << fixed << setprecision(1) << "Down arrow pressed. Seperation Distance is set to "
                 << seperationDistance << "." << endl;
            sprintf(message, "Down arrow pressed. Seperation Distance is set to %.1f.", seperationDistance);
            break;
        }
            // Left arrow
        case GLUT_KEY_LEFT: {
            if (alignmentDistance > 0.1f) {
                alignmentDistance -= PARA_SPEED;
            }
            cout << fixed << setprecision(1) << "Left arrow pressed. Alignment Distance is set to " << alignmentDistance
                 << "." << endl;
            sprintf(message, "Left arrow pressed. Alignment Distance is set to %.1f.", alignmentDistance);
            break;
        }
            // Right arrow
        case GLUT_KEY_RIGHT: {
            if (alignmentDistance < 99.9f) {
                alignmentDistance += PARA_SPEED;
            }
            cout << fixed << setprecision(1) << "Left arrow pressed. Alignment Distance is set to " << alignmentDistance
                 << "." << endl;
            sprintf(message, "Left arrow pressed. Alignment Distance is set to %.1f.", alignmentDistance);
            break;
        }
            // Home
        case GLUT_KEY_HOME: {
            if (cohesionDistance < 99.9f) {
                cohesionDistance += PARA_SPEED;
            }
            cout << fixed << setprecision(1) << "Home pressed. Cohesion Distance is set to " << cohesionDistance << "."
                 << endl;
            sprintf(message, "Home pressed. Cohesion Distance is set to %.1f.", cohesionDistance);
            break;
        }
            // End
        case GLUT_KEY_END: {
            if (cohesionDistance > 0.1f) {
                cohesionDistance -= PARA_SPEED;
            }
            cout << fixed << setprecision(1) << "End pressed. Cohesion Distance is set to " << cohesionDistance << "."
                 << endl;
            sprintf(message, "End pressed. Cohesion Distance is set to %.1f.", cohesionDistance);
            break;
        }
        default:
            break;
    }
}

void PrintStatus() {
    static int frame = 0;
    static int currenttime;
    static int timebase = 0;
    static char fpstext[50];
    char *c;
    char cameraPositionMessage[50];
    char targetPositionMessage[50];
    char cameraPolarPositonMessage[50];
    char separationDistanceMessage[50];
    char alignmentDistanceMessage[50];
    char cohesionDistanceMessage[50];

    frame++;
    currenttime = glutGet(GLUT_ELAPSED_TIME);
    if (currenttime - timebase > 1000) {
        sprintf(fpstext, "FPS:%4.2f",
                frame * 1000.0 / (currenttime - timebase));
        timebase = currenttime;
        frame = 0;
    }

    sprintf(cameraPositionMessage, "Camera Position  %2.1f   %2.1f   %2.1f",
            camera[X], camera[Y], camera[Z]);
    sprintf(targetPositionMessage, "Target Position     %2.1f   %2.1f   %2.1f",
            target[X], target[Y], target[Z]);
    sprintf(cameraPolarPositonMessage, "Camera Polar      %2.1f   %2.3f   %2.3f",
            polar[R], polar[A], polar[T]);
    sprintf(separationDistanceMessage, "Separation Distance           %2.1f",
            seperationDistance);
    sprintf(alignmentDistanceMessage, "Alignment  Distance           %2.1f",
            alignmentDistance);
    sprintf(cohesionDistanceMessage, "Cohesion  Distance           %2.1f",
            cohesionDistance);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);                    // 不受灯光影响
    glMatrixMode(GL_PROJECTION);            // 选择投影矩阵
    glPushMatrix();                            // 保存原矩阵
    glLoadIdentity();                        // 装入单位矩阵
    glOrtho(-640, 640, -360, 360, -1, 1);    // 设置裁减区域
    glMatrixMode(GL_MODELVIEW);                // 选择Modelview矩阵
    glPushMatrix();                            // 保存原矩阵
    glLoadIdentity();                        // 装入单位矩阵
    glPushAttrib(GL_LIGHTING_BIT);
    glRasterPos2f(20 - window[W] / 2, window[H] / 2 - 20);
    for (c = fpstext; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glRasterPos2f(window[W] / 2 - 240, window[H] / 2 - 20);
    for (c = cameraPositionMessage; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    glRasterPos2f(window[W] / 2 - 240, window[H] / 2 - 55);
    for (c = targetPositionMessage; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    glRasterPos2f(window[W] / 2 - 240, window[H] / 2 - 90);
    for (c = cameraPolarPositonMessage; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    glRasterPos2f(window[W] / 2 - 240, window[H] / 2 - 125);
    for (c = separationDistanceMessage; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    glRasterPos2f(window[W] / 2 - 240, window[H] / 2 - 160);
    for (c = alignmentDistanceMessage; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    glRasterPos2f(window[W] / 2 - 240, window[H] / 2 - 195);
    for (c = cohesionDistanceMessage; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    glRasterPos2f(20 - window[W] / 2, 20 - window[H] / 2);
    for (c = message; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);            // 选择投影矩阵
    glPopMatrix();                            // 重置为原保存矩阵
    glMatrixMode(GL_MODELVIEW);                // 选择Modelview矩阵
    glPopMatrix();                            // 重置为原保存矩阵
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void setupVBO() {
    bird = new VBOBird(base);
}

void setupShader() {
    try {
        birdShader.compileShader("bird.vert");
        birdShader.compileShader("bird.frag");
        birdShader.link();
        computeShader.compileShader("compute.vert");
        computeShader.compileShader("compute.frag");
        computeShader.link();
    } catch (GLSLProgramException &e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    GLuint birdShaderProgram = birdShader.getProgram();
    colorTypeBird[0] = glGetSubroutineIndex(birdShaderProgram, GL_VERTEX_SHADER, "directionalColor");               // 0
    colorTypeBird[1] = glGetSubroutineIndex(birdShaderProgram, GL_VERTEX_SHADER, "randomColor");                    // 1
    positionGetterBird[0] = glGetSubroutineIndex(birdShaderProgram, GL_VERTEX_SHADER, "getUpperPosition");          // 2
    positionGetterBird[1] = glGetSubroutineIndex(birdShaderProgram, GL_VERTEX_SHADER, "getLowerPosition");          // 3
    velocityGetterBird[0] = glGetSubroutineIndex(birdShaderProgram, GL_VERTEX_SHADER, "getUpperVelocity");          // 4
    velocityGetterBird[1] = glGetSubroutineIndex(birdShaderProgram, GL_VERTEX_SHADER, "getLowerVelocity");          // 5
    GLuint computeShaderProgram = computeShader.getProgram();
    positionGetterCompute[0] = glGetSubroutineIndex(computeShaderProgram, GL_FRAGMENT_SHADER, "getUpperPosition");  // 0
    positionGetterCompute[1] = glGetSubroutineIndex(computeShaderProgram, GL_FRAGMENT_SHADER, "getLowerPosition");  // 1
    velocityGetterCompute[0] = glGetSubroutineIndex(computeShaderProgram, GL_FRAGMENT_SHADER, "getUpperVelocity");  // 2
    velocityGetterCompute[1] = glGetSubroutineIndex(computeShaderProgram, GL_FRAGMENT_SHADER, "getLowerVelocity");  // 3
    positionSetterCompute[0] = glGetSubroutineIndex(computeShaderProgram, GL_FRAGMENT_SHADER, "setUpperPosition");  // 4
    positionSetterCompute[1] = glGetSubroutineIndex(computeShaderProgram, GL_FRAGMENT_SHADER, "setLowerPosition");  // 5
    velocitySetterCompute[0] = glGetSubroutineIndex(computeShaderProgram, GL_FRAGMENT_SHADER, "setUpperVelocity");  // 6
    velocitySetterCompute[1] = glGetSubroutineIndex(computeShaderProgram, GL_FRAGMENT_SHADER, "setLowerVelocity");  // 7
}

void updateBirdShaderUniform() {
    mat4 model;
    mat4 view;
    mat4 projection;
    view = glm::lookAt(vec3(camera[X], camera[Y], camera[Z]), vec3(target[X], target[Y], target[Z]),
                       vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(45.0f, static_cast<float>(window[W] * 1.0 / window[H]), 0.1f, 30000.0f);
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.7));
    mat4 mv = view * model;
    birdShader.setUniform("base", (GLfloat) base);
    birdShader.setUniform("ModelMatrix", model);
    birdShader.setUniform("ViewMatrix", view);
    birdShader.setUniform("ProjectionMatrix", projection);
    GLuint subroutines[3] = {
            colorTypeBird[(int) bRandomColor], positionGetterBird[activeRegion], velocityGetterBird[activeRegion]
    };
    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 3, subroutines);
}

void updateComputeShaderUniform() {
    static vec3 predator(1, 1, 1);
    static double last = clock();
    static double now = 0;
    static float delta = 0;
    now = clock();
    delta = static_cast<float>((now - last) / 1000.0);
    if (delta > 1) delta = 1;
    last = now;
    predator = vec3(mouse[X], mouse[Y], 0);

    computeShader.setUniform("delta", delta);
    computeShader.setUniform("MVP", mat4(1.0f) * mat4(1.0f) * mat4(1.0f));
    computeShader.setUniform("seperationDistance", seperationDistance);
    computeShader.setUniform("alignmentDistance", alignmentDistance);
    computeShader.setUniform("cohesionDistance", cohesionDistance);
    computeShader.setUniform("base", (GLfloat) base);
    computeShader.setUniform("predator", predator);
    mouse[X] = mouse[Y] = 1.0f;
    GLuint subroutines[4] = {
            positionGetterCompute[activeRegion], velocityGetterCompute[activeRegion],
            positionSetterCompute[1 - activeRegion], velocitySetterCompute[1 - activeRegion]
    };
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 4, subroutines);
}

void setupTexture() {
    /////////Create the compute texture/////////
    glGenTextures(1, &computeTexture);
    glBindTexture(GL_TEXTURE_2D, computeTexture);

    auto *initialData = new float[base * 2 * base * 2 * 4];
    auto *random = new float[6];
    for (int i = 0; i < base * base; i++) {
        for (int j = 0; j < 6; j++) {
            random[j] = static_cast<float>(rand() % 10000 / 10000.0 - 0.5);
        }
        int col = i % base;
        int row = i / base;

        // Position data
        initialData[4 * row * 2 * base + 4 * col] = random[0] * BOUNDS;
        initialData[4 * row * 2 * base + 4 * col + 1] = random[1] * BOUNDS;
        initialData[4 * row * 2 * base + 4 * col + 2] = random[2] * BOUNDS;
        initialData[4 * row * 2 * base + 4 * col + 3] = 1;
        // Velocity data
        initialData[4 * row * 2 * base + 4 * col + base * 4] = random[3];
        initialData[4 * row * 2 * base + 4 * col + base * 4 + 1] = random[4];
        initialData[4 * row * 2 * base + 4 * col + base * 4 + 2] = random[5];
        initialData[4 * row * 2 * base + 4 * col + base * 4 + 3] = 1;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, base * 2, base * 2, 0, GL_RGBA, GL_FLOAT, initialData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    delete[] initialData;
    delete[] random;
    /////////Create the coord texture/////////
    glGenTextures(1, &coordTexture);
    glBindTexture(GL_TEXTURE_2D, coordTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, base, base, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    /////////////Bind image//////////////////
    glBindImageTexture(0, computeTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void setupFBO() {
    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glGenFramebuffers(1, &fboHandle);
    // Bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, coordTexture, 0);

    // Set the targets for the fragment output variables
    glDrawBuffers(1, drawBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ///////////////////////////////////////////
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE) {
        cout << "Framebuffer is complete" << endl;
    } else {
        cout << "Framebuffer error: " << result << endl;
    }

    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void setupVAO() {
    // Array for full-screen quad
    GLfloat verts[] = {
            -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };

    // Set up the buffers
    GLuint handle[1];
    glGenBuffers(1, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    // Set up the vertex array object
    glGenVertexArrays(1, &fsQuad);
    glBindVertexArray(fsQuad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindVertexArray(0);
}

void init() {
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        cerr << "Error occurred when initializing GLEW: " << glewGetErrorString(glewErr) << endl;
        exit(1);
    }
    if (!glewIsSupported("GL_VERSION_4_3")) {
        cerr << "OpenGL 4.3 is not supported" << endl;
        exit(1);
    }

    string stringBaseOfBirds;
    cout << "Enter the base number of birds you want to create (like 2, 4, 8, 16 or 32)" << endl;
    getline(cin, stringBaseOfBirds);
    int decision = string2int(stringBaseOfBirds);
    if (decision != 2 && decision != 4 && decision != 8 && decision != 16 && decision != 32) {
        cout << "Invalid input: " << stringBaseOfBirds << "." << endl;
        cout << "Default base will be used: " << DEFAULT_BASE_OF_BIRDS << endl;
        base = DEFAULT_BASE_OF_BIRDS;
    } else {
        cout << "Your decision: " << decision << endl;
        base = decision;
    }

    // Set the background color - white
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);

    setupShader();
    setupVBO();
    setupTexture();
    setupFBO();
    setupVAO();
}