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
GLuint positionTexture;
GLuint velocityTexture;
GLuint fboHandle;
GLuint fsQuad;
mat4 model;
mat4 view;
mat4 projection;
vec3 predator(1000, 1000, 1000);
GLfloat camera[3] = {0, 0, 350};                    // Position of camera
GLfloat target[3] = {0, 0, 0};                    // Position of target of camera
GLfloat camera_polar[3] = {350, -1.57f, 0};            // Polar coordinates of camera
bool bMsaa = false;                            // Switch of Multisampling anti-alias
bool bShader = true;                       // Switch of shader
bool bcamera = true;                        // Switch of camera/target control
bool bfocus = true;                            // Status of window focus
bool bmouse = false;                        // Whether mouse postion should be moved
int fpsmode = 2;                                    // 0:off, 1:on, 2:waiting
int window[2] = {1280, 720};                        // Window size
int windowcenter[2];                                // Center of this window, to be updated
int time_0 = clock();
int time_1;
float delta;
float seperationDistance = 20.0f;
float alignmentDistance = 20.0f;
float cohesionDistance = 20.0f;
char message[70] = "Welcome!";                        // Message string to be shown
//int focus = NONE;									// Focus object by clicking RMB

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

    glMatrixMode(GL_PROJECTION);            // Select The Projection Matrix
    glLoadIdentity();                        // Reset The Projection Matrix
    gluPerspective(45.0f, 1.7778f, 0.1f, 30000.0f);    // 1.7778 = 1280 / 720
    glMatrixMode(GL_MODELVIEW);                // Select The Modelview Matrix
}

void Redraw() {
    ///////////////Update texture//////////////
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glDisable(GL_DEPTH_TEST);
    computeShader.use();
    updateComputeShaderUniform();
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    computeShader.disable();
    glFlush();
    ///////////////////Draw the birds///////////////////
    birdShader.use();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_MULTISAMPLE_ARB);
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

void ProcessMouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        bMsaa = !bMsaa;
        cout << "LMB pressed. Switch on/off multisampling anti-alias.\n" << endl;
        strcpy(message, "LMB pressed. Switch on/off multisampling anti-alias.");
        glutPostRedisplay();
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && fpsmode) {
        bShader = !bShader;
    }
}

void ProcessMouseMove(int x, int y) {
    cout << "Mouse moves to (" << x << ", " << y << ")" << endl;
    if (fpsmode) {
        // Track target and reverse mouse moving to center point.
        if (fpsmode == 2) {
            // 鼠标位置居中，为确保在glutPositionWindow()之后执行
            updateWindowcenter(window, windowcenter);
            SetCursorPos(windowcenter[X], windowcenter[Y]);
            glutSetCursor(GLUT_CURSOR_NONE);
            fpsmode = 1;
            return;
        }
        if (x < window[W] * 0.25) {
            x += window[W] * 0.5;
            bmouse = !bmouse;
        } else if (x > window[W] * 0.75) {
            x -= window[W] * 0.5;
            bmouse = !bmouse;
        }
        if (y < window[H] * 0.25) {
            y = static_cast<int>(window[H] * 0.25);
            bmouse = !bmouse;
        } else if (y > window[H] * 0.75) {
            y = static_cast<int>(window[H] * 0.75);
            bmouse = !bmouse;
        }
        // 将新坐标与屏幕中心的差值换算为polar的变化
        camera_polar[A] = static_cast<GLfloat>((window[W] / 2 - x) * (180 / 180.0 * PI) / (window[W] / 4.0) *
                                               PANNING_PACE);            // Delta pixels * 180 degrees / (1/4 width) * PANNING_PACE
        camera_polar[T] = static_cast<GLfloat>((window[H] / 2 - y) * (90 / 180.0 * PI) / (window[H] / 4.0) *
                                               PANNING_PACE);            // Delta pixels * 90 degrees / (1/4 height) * PANNING_PACE
        // 移动光标
        if (bmouse) {
            SetCursorPos(glutGet(GLUT_WINDOW_X) + x, glutGet(GLUT_WINDOW_Y) + y);
            bmouse = !bmouse;
        }
        // 更新摄像机目标
        updateTarget(camera, target, camera_polar);
    }
}

void ProcessFocus(int state) {
    if (state == GLUT_LEFT) {
        bfocus = GL_FALSE;
        cout << "Focus is on other window." << endl;
    } else if (state == GLUT_ENTERED) {
        bfocus = GL_TRUE;
        cout << "Focus is on this window." << endl;
    }
}

void ProcessNormalKey(unsigned char k, int x, int y) {
    switch (k) {
        // 退出程序
        case 27: {
            cout << "Bye." << endl;
            exit(0);
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
            cameraMakeZero(camera, target, camera_polar);
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
                saveCamera(camera, target, camera_polar);
                camera[X] -= cos(camera_polar[A]) * MOVING_PACE;
                camera[Z] += sin(camera_polar[A]) * MOVING_PACE;
                target[X] -= cos(camera_polar[A]) * MOVING_PACE;
                target[Z] += sin(camera_polar[A]) * MOVING_PACE;
            } else {
                if (bcamera) {
                    camera_polar[A] -= OBSERVING_PACE * 0.1;
                    updateCamera(camera, target, camera_polar);
                    cout << fixed << setprecision(1) << "A pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                } else {
                    target[X] -= OBSERVING_PACE;
                    updatePolar(camera, target, camera_polar);
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
                saveCamera(camera, target, camera_polar);
                camera[X] += cos(camera_polar[A]) * MOVING_PACE;
                camera[Z] -= sin(camera_polar[A]) * MOVING_PACE;
                target[X] += cos(camera_polar[A]) * MOVING_PACE;
                target[Z] -= sin(camera_polar[A]) * MOVING_PACE;
            } else {
                if (bcamera) {
                    camera_polar[A] += OBSERVING_PACE * 0.1;
                    updateCamera(camera, target, camera_polar);
                    cout << fixed << setprecision(1) << "D pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                } else {
                    target[X] += OBSERVING_PACE;
                    updatePolar(camera, target, camera_polar);
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
                saveCamera(camera, target, camera_polar);
                camera[X] -= sin(camera_polar[A]) * MOVING_PACE;
                camera[Z] -= cos(camera_polar[A]) * MOVING_PACE;
                target[X] -= sin(camera_polar[A]) * MOVING_PACE;
                target[Z] -= cos(camera_polar[A]) * MOVING_PACE;
            } else {
                if (bcamera) {
                    camera[Y] += OBSERVING_PACE;
                    cout << fixed << setprecision(1) << "W pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                } else {
                    target[Y] += OBSERVING_PACE;
                    updatePolar(camera, target, camera_polar);
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
                saveCamera(camera, target, camera_polar);
                camera[X] += sin(camera_polar[A]) * MOVING_PACE;
                camera[Z] += cos(camera_polar[A]) * MOVING_PACE;
                target[X] += sin(camera_polar[A]) * MOVING_PACE;
                target[Z] += cos(camera_polar[A]) * MOVING_PACE;
            } else {
                if (bcamera) {
                    camera[Y] -= OBSERVING_PACE;
                    cout << fixed << setprecision(1) << "S pressed.\n\tPosition of camera is set to (" <<
                         camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
                    strcpy(message, "S pressed. Watch carefully!");
                } else {
                    target[Y] -= OBSERVING_PACE;
                    updatePolar(camera, target, camera_polar);
                    cout << fixed << setprecision(1) << "S pressed.\n\tPosition of camera target is set to (" <<
                         target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
                }
            }
            break;
        }
        case 'Q':
        case 'q': {
            if (bcamera) {
                strcpy(message, "Q pressed. Camera is moved...nearer!");
                camera_polar[R] *= 0.95;
                updateCamera(camera, target, camera_polar);
                cout << fixed << setprecision(1) << "Q pressed.\n\tPosition of camera is set to (" <<
                     camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
            } else {
                strcpy(message, "Q pressed. Camera target is moving towards +Z!");
                target[Z] += OBSERVING_PACE;
                updatePolar(camera, target, camera_polar);
                cout << fixed << setprecision(1) << "Q pressed.\n\tPosition of camera target is set to (" <<
                     target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
            }
            break;
        }
        case 'E':
        case 'e': {
            if (bcamera) {
                strcpy(message, "E pressed. Camera is moved...farther!");
                camera_polar[R] *= 1.05;
                updateCamera(camera, target, camera_polar);
                cout << fixed << setprecision(1) << "E pressed.\n\tPosition of camera is set to (" <<
                     camera[X] << ", " << camera[Y] << ", " << camera[Z] << ")." << endl;
            } else {
                strcpy(message, "E pressed. Camera target is moving towards -Z!");
                target[Z] -= OBSERVING_PACE;
                updatePolar(camera, target, camera_polar);
                cout << fixed << setprecision(1) << "E pressed.\n\tPosition of camera target is set to (" <<
                     target[X] << ", " << target[Y] << ", " << target[Z] << ")." << endl;
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
        default:
            break;
    }
}

void ProcessSpecialKey(int k, int x, int y) {
    switch (k) {
        // Up arrow
        case GLUT_KEY_UP: {
            if (seperationDistance < 99.9f) {
                seperationDistance += 0.1f;
            }
            cout << fixed << setprecision(1) << "Up arrow pressed. Seperation Distance is set to " << seperationDistance << "." << endl;
            sprintf(message,  "Up arrow pressed. Seperation Distance is set to %.1f.", seperationDistance);
            break;
        }
            // Down arrow
        case GLUT_KEY_DOWN: {
            if (seperationDistance > 0.1f) {
                seperationDistance -= 0.1f;
            }
            cout << fixed << setprecision(1) << "Down arrow pressed. Seperation Distance is set to " << seperationDistance << "." << endl;
            sprintf(message,  "Down arrow pressed. Seperation Distance is set to %.1f.", seperationDistance);
            break;
        }
            // Left arrow
        case GLUT_KEY_LEFT: {
            if (alignmentDistance > 0.1f) {
                alignmentDistance -= 0.1f;
            }
            cout << fixed << setprecision(1) << "Left arrow pressed. Alignment Distance is set to " << alignmentDistance << "." << endl;
            sprintf(message,  "Left arrow pressed. Alignment Distance is set to %.1f.", alignmentDistance);
            break;
        }
            // Right arrow
        case GLUT_KEY_RIGHT: {
            if (alignmentDistance < 99.9f) {
                alignmentDistance += 0.1f;
            }
            cout << fixed << setprecision(1) << "Left arrow pressed. Alignment Distance is set to " << alignmentDistance << "." << endl;
            sprintf(message,  "Left arrow pressed. Alignment Distance is set to %.1f.", alignmentDistance);
            break;
        }
            // Home
        case GLUT_KEY_HOME: {
            if (cohesionDistance < 99.9f) {
                cohesionDistance += 0.1f;
            }
            cout << fixed << setprecision(1) << "Home pressed. Cohesion Distance is set to " << cohesionDistance << "." << endl;
            sprintf(message,  "Home pressed. Cohesion Distance is set to %.1f.", cohesionDistance);
            break;
        }
            // End
        case GLUT_KEY_END: {
            if (cohesionDistance > 0.1f) {
                cohesionDistance -= 0.1f;
            }
            cout << fixed << setprecision(1) << "End pressed. Cohesion Distance is set to " << cohesionDistance << "." << endl;
            sprintf(message,  "End pressed. Cohesion Distance is set to %.1f.", cohesionDistance);
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
            camera_polar[R], camera_polar[A], camera_polar[T]);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);                    // 不受灯光影响
    glMatrixMode(GL_PROJECTION);            // 选择投影矩阵
    glPushMatrix();                            // 保存原矩阵
    glLoadIdentity();                        // 装入单位矩阵
    glOrtho(-window[W] / 2, window[W] / 2, -window[H] / 2, window[H] / 2, -1, 1);    // 设置裁减区域
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

void initVBO() {
    bird = new VBOBird(32);
}

void setShader() {
}

void updateBirdShaderUniform() {
    birdShader.use();
    view = glm::lookAt(vec3(camera[X], camera[Y], camera[Z]), vec3(target[X], target[Y], target[Z]),
                       vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(45.0f, 1.7778f, 0.1f, 30000.0f);
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 mv = view * model;
    birdShader.setUniform("ModelMatrix", model);
    birdShader.setUniform("ViewMatrix", view);
    birdShader.setUniform("ProjectionMatrix", projection);
    birdShader.setUniform("ModelViewMatrix", mv);
    birdShader.setUniform("MVP", projection * mv);
}

void updateComputeShaderUniform() {
    time_1 = clock();
    delta = static_cast<float>((time_1 - time_0) / 1000.0);
    if (delta > 1) {
        delta = 1;
    }
    time_0 = time_1;
    computeShader.setUniform("delta", delta);
    computeShader.setUniform("seperationDistance", 20.0f);
    computeShader.setUniform("alignmentDistance", 20.0f);
    computeShader.setUniform("cohesionDistance", 20.0f);
    computeShader.setUniform("predator", vec3(1000, 1000, 1000));
    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);
    mat4 mv = view * model;
    computeShader.setUniform("ModelMatrix", model);
    computeShader.setUniform("ViewMatrix", view);
    computeShader.setUniform("ProjectionMatrix", projection);
    computeShader.setUniform("ModelViewMatrix", mv);
    computeShader.setUniform("MVP", projection * mv);
}

void setupTexture() {
    glGenTextures(1, &positionTexture);
    glGenTextures(1, &velocityTexture);

    // Create the position texture
    glBindTexture(GL_TEXTURE_2D, positionTexture);

    GLfloat *positionData = new GLfloat[1024 * 4];
    for (int i = 0; i < 1024; i++) {
        GLfloat x = static_cast<GLfloat>(rand() % 10000 / 10000.0 * BOUNDS - BOUNDS / 2);
        GLfloat y = static_cast<GLfloat>(rand() % 10000 / 10000.0 * BOUNDS - BOUNDS / 2);
        GLfloat z = static_cast<GLfloat>(rand() % 10000 / 10000.0 * BOUNDS - BOUNDS / 2);

        positionData[i * 4] = x;
        positionData[i * 4 + 1] = y;
        positionData[i * 4 + 2] = z;
//        positionData[i * 4] = i - 512;
//        positionData[i * 4 + 1] = i - 512;
//        positionData[i * 4 + 2] = i - 512;
        positionData[i * 4 + 3] = 1;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 32, 32, 0, GL_RGBA, GL_FLOAT, positionData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // Create the position texture
    glBindTexture(GL_TEXTURE_2D, velocityTexture);

    GLfloat *velocityData = new GLfloat[1024 * 4];
    for (int i = 0; i < 1024; i++) {
        GLfloat x = static_cast<GLfloat>(rand() % 10000 / 10000.0 - 0.5);
        GLfloat y = static_cast<GLfloat>(rand() % 10000 / 10000.0 - 0.5);
        GLfloat z = static_cast<GLfloat>(rand() % 10000 / 10000.0 - 0.5);

        velocityData[i * 4] = x;
        velocityData[i * 4 + 1] = y;
        velocityData[i * 4 + 2] = z;
        velocityData[i * 4 + 3] = 1;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 32, 32, 0, GL_RGBA, GL_FLOAT, velocityData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    ///////////////////////////////////////////
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positionTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, velocityTexture);
}

void setupFBO() {
    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glGenFramebuffers(1, &fboHandle);
    // Bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, velocityTexture, 0);

    // Set the targets for the fragment output variables
    glDrawBuffers(2, drawBuffers);
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
    GLfloat tc[] = {
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    // Set up the buffers

    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    // Set up the vertex array object

    glGenVertexArrays(1, &fsQuad);
    glBindVertexArray(fsQuad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint) 2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);  // Texture coordinates

    glBindVertexArray(0);
}
