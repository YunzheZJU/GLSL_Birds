//
// System.cpp
// Processing system display and control
// Created by Yunzhe on 2017/12/4.
//

#include "system.h"
#include "glutils.h"

Shader birdShader = Shader();
Shader computeShader = Shader();
//VBOPlane *plane;
//VBOTeapot *teapot;
//VBOTorus *torus;
//VBOCube *cube;
VBOBird *bird;
//GLuint fboHandle;
//GLuint pass1Index;
//GLuint pass2Index;
GLuint positionTexture[2];
GLuint velocityTexture[2];
GLuint positionComputer;
GLuint velocityComputer;
GLuint positionFBO[2];
GLuint velocityFBO[2];
GLuint fsQuad;
mat4 model;
mat4 view;
mat4 projection;
GLfloat camera[3] = {1, 2, 350};                    // Position of camera
GLfloat target[3] = {0, 0, 0};                    // Position of target of camera
GLfloat camera_polar[3] = {5, -1.57f, 0};            // Polar coordinates of camera
bool bMsaa = false;                            // Switch of Multisampling anti-alias
bool bShader = true;                       // Switch of shader
GLfloat camera_locator[3] = {0, -5, 10};            // Position of shadow of camera
bool bcamera = true;                        // Switch of camera/target control
bool bfocus = true;                            // Status of window focus
bool bmouse = false;                        // Whether mouse postion should be moved
int fpsmode = 2;                                    // 0:off, 1:on, 2:waiting
int window[2] = {1280, 720};                        // Window size
int windowcenter[2];                                // Center of this window, to be updated
int time_0 = clock();
int time_1;
int currentTarget = 1;
float delta;
char message[70] = "Welcome!";                        // Message string to be shown
//int focus = NONE;									// Focus object by clicking RMB
GLfloat angle = 0.0f;

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
    ///////////////Update position texture//////////////
    computeShader.use();
    updateComputeShaderUniform();
    glBindFramebuffer(GL_FRAMEBUFFER, positionFBO[currentTarget]);
    // Render filter Image
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positionTexture[1 - currentTarget]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, velocityTexture[1 - currentTarget]);

    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &positionComputer);

    // Render the full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glFlush();
    ///////////////Update velocity texture//////////////
    computeShader.use();
    glBindFramebuffer(GL_FRAMEBUFFER, velocityFBO[currentTarget]);
    // Render filter Image
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positionTexture[1 - currentTarget]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, velocityTexture[1 - currentTarget]);

    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &velocityComputer);
//    updateComputeShaderUniform();

    // Render the full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glFlush();
    computeShader.disable();
    ///////////////////Draw the birds///////////////////
    birdShader.use();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Render scene
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();                        // Reset The Current Modelview Matrix
    // 必须定义，以在固定管线中绘制物体
    gluLookAt(camera[X], camera[Y], camera[Z],
              target[X], target[Y], target[Z],
              0, 1, 0);                            // Define the view matrix
//    if (bMsaa) {
    glEnable(GL_MULTISAMPLE_ARB);
//    } else {
//        glDisable(GL_MULTISAMPLE_ARB);
//    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positionTexture[1 - currentTarget]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, velocityTexture[1 - currentTarget]);
//    angle += 0.5f;
//    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);
    glEnable(GL_DEPTH_TEST);
    // Draw something here
    updateShaderMVP();
//    cube->render();
    bird->render();
//    DrawScene();

    birdShader.disable();
    // Show fps, message and other information
    PrintStatus();

    //////////////////////End////////////////////////
    glutSwapBuffers();

//    // Swap the textures
//    GLuint temp = positionTexture[1];
//    positionTexture[1] = positionTexture[0];
//    positionTexture[0] = temp;
//    temp = velocityTexture[1];
//    velocityTexture[1] = velocityTexture[0];
//    velocityTexture[0] = temp;

    currentTarget = 1 - currentTarget;

    GLUtils::checkForOpenGLError(__FILE__, __LINE__);
}

void ProcessMouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        bMsaa = !bMsaa;
        cout << "LMB pressed. Switch on/off multisampling anti-alias.\n" << endl;
        strcpy(message, "LMB pressed. Switch on/off multisampling anti-alias.");
        glutPostRedisplay();
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && fpsmode) {
//        processPick(window);
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
            // 光源颜色分量
        case 'R':
        case 'r': {
            cout << "R pressed." << endl;
            currentColor[0] -= 0.1f;
            if (currentColor[0] < 0) {
                currentColor[0] = 1.0f;
            }
            break;
        }
        case 'G':
        case 'g': {
            cout << "G pressed." << endl;
            currentColor[1] -= 0.1f;
            if (currentColor[1] < 0) {
                currentColor[1] = 1.0f;
            }
            break;
        }
        case 'B':
        case 'b': {
            cout << "B pressed." << endl;
            currentColor[2] -= 0.1f;
            if (currentColor[2] < 0) {
                currentColor[2] = 1.0f;
            }
            break;
        }
            // 光源强度（衰减）
        case '+': {
            cout << "+ pressed." << endl;
            constantattenuation -= 0.1f;
            cout << constantattenuation << endl;
            break;
        }
        case '-': {
            cout << "- pressed." << endl;
            constantattenuation += 0.1f;
            cout << constantattenuation << endl;
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
        case 101: {
            lightPosition0[Y] += 5;
            break;
        }
            // Down arrow
        case 103: {
            if (lightPosition0[Y] >= 10) {
                lightPosition0[Y] -= 5;
            }
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
//    plane = new VBOPlane(50.0f, 50.0f, 1, 1);
//    teapot = new VBOTeapot(14, glm::mat4(1.0f));
//    torus = new VBOTorus(0.7f * 2, 0.3f * 2, 50, 50);
//    cube = new VBOCube();
    bird = new VBOBird(32);
}

void setShader() {
    GLuint shaderProgram = computeShader.getProgram();
    positionComputer = glGetSubroutineIndex(shaderProgram, GL_FRAGMENT_SHADER, "position");
    velocityComputer = glGetSubroutineIndex(shaderProgram, GL_FRAGMENT_SHADER, "velocity");

//    shader.setUniform("Ka", 0.9f, 0.5f, 0.3f);
//    shader.setUniform("Kd", 0.9f, 0.5f, 0.3f);
//    shader.setUniform("Ks", 0.8f, 0.8f, 0.8f);
//    shader.setUniform("Shininess", 100.0f);
//    shader.setUniform("EdgeThreshold", 0.05f);
//    shader.setUniform("Width", window[W]);
//    shader.setUniform("Height", window[H]);
//    shader.setUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));

//    updateShaderMVP();
}

void updateMVPZero() {
//    shader.setUniform("Light.Position", view * vec4(0.0f, 0.0f, 10.0f, 1.0f));
}

void updateMVPOne() {
//    model = glm::translate(model, vec3(-2.0f, -1.5f, 0.0f));
//    model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 1.0f, 0.0f));
//    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));

//    shader.setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
//    shader.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
//    shader.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
//    shader.setUniform("Material.Shininess", 100.0f);

    updateShaderMVP();
}

void updateMVPTwo() {
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -2.0f, 0.0f));
//    model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 1.0f, 0.0f));
//    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));

    birdShader.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    birdShader.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    birdShader.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    birdShader.setUniform("Material.Shininess", 1.0f);

    updateShaderMVP();
}

void updateMVPThree() {
    model = mat4(1.0f);
    model = glm::translate(model, vec3(2.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angle), vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));

    birdShader.setUniform("Material.Kd", 0.9f, 0.5f, 0.2f);
    birdShader.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    birdShader.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    birdShader.setUniform("Material.Shininess", 100.0f);

    updateShaderMVP();
}

void updateShaderMVP() {
    birdShader.use();
    view = glm::lookAt(vec3(camera[X], camera[Y], camera[Z]), vec3(target[X], target[Y], target[Z]),
                       vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(45.0f, 1.7778f, 0.1f, 30000.0f);
    model = mat4(1.0f);
    mat4 mv = view * model;
    birdShader.setUniform("ModelMatrix", model);
    birdShader.setUniform("ViewMatrix", view);
    birdShader.setUniform("ProjectionMatrix", projection);
    birdShader.setUniform("ModelViewMatrix", mv);
    birdShader.setUniform("MVP", projection * mv);
}

void updateComputeShaderUniform() {
    time_1 = clock();
    delta = (time_1 - time_0) / 1000.0;
    if (delta > 1) {
        delta = 1;
    }
    time_0 = time_1;
    computeShader.setUniform("seperationDistance", 20.0f);
    computeShader.setUniform("alignmentDistance", 40.0f);
    computeShader.setUniform("cohesionDistance", 20.0f);
    computeShader.setUniform("predator", vec3(0, 0, 0));
    computeShader.setUniform("delta", delta);
}

void setupTexture() {
    glGenTextures(2, positionTexture);
    glGenTextures(2, velocityTexture);

    for (int num = 0; num < 2; num++) {
        // Create the position texture
        glBindTexture(GL_TEXTURE_2D, positionTexture[num]);

        GLfloat *positionData = new GLfloat[1024 * 4];
        for (int i = 0; i < 1024; i++) {
            GLfloat x = static_cast<GLfloat>(rand() % 10000 / 10000.0 * BOUNDS - BOUNDS / 2);
            GLfloat y = static_cast<GLfloat>(rand() % 10000 / 10000.0 * BOUNDS - BOUNDS / 2);
            GLfloat z = static_cast<GLfloat>(rand() % 10000 / 10000.0 * BOUNDS - BOUNDS / 2);

            positionData[i * 4] = x;
            positionData[i * 4 + 1] = y;
            positionData[i * 4 + 2] = z;
            positionData[i * 4 + 3] = 1;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 32, 32, 0, GL_RGBA, GL_FLOAT, positionData);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        // Create the position texture
        glBindTexture(GL_TEXTURE_2D, velocityTexture[num]);

        GLfloat *velocityData = new GLfloat[1024 * 4];
        for (int i = 0; i < 1024; i++) {
            GLfloat x = static_cast<GLfloat>(rand() % 10000 / 10000.0 - 0.5);
            GLfloat y = static_cast<GLfloat>(rand() % 10000 / 10000.0 - 0.5);
            GLfloat z = static_cast<GLfloat>(rand() % 10000 / 10000.0 - 0.5);

            velocityData[i * 4] = x * 10;
            velocityData[i * 4 + 1] = y * 10;
            velocityData[i * 4 + 2] = z * 10;
            velocityData[i * 4 + 3] = 1;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 32, 32, 0, GL_RGBA, GL_FLOAT, velocityData);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    }
    float *data = new float[32 * 32 * 4];
    data[10] = 1.0f;
    glBindTexture(GL_TEXTURE_2D, positionTexture[1]);
    glReadPixels(0, 0, 32, 32, GL_RGBA16F, GL_FLOAT, data);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA16F, GL_FLOAT, data);
    float a = data[0];
    float b = data[10];
    float c = data[100];
}

void setupFBO() {
    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glGenFramebuffers(2, positionFBO);
    glGenFramebuffers(2, velocityFBO);
    //////////////Position FBO #0: render to texture 0//////////////////
    // Bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, positionFBO[0]);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionTexture[0], 0);

    // Set the targets for the fragment output variables
    glDrawBuffers(1, drawBuffers);
    //////////////Position FBO #1: render to texture 1//////////////////
    // Bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, positionFBO[1]);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionTexture[1], 0);

    // Set the targets for the fragment output variables
    glDrawBuffers(1, drawBuffers);
    ///////////////Velocity FBO #0: render to texture 0////////////////////
    // Bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, velocityFBO[0]);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, velocityTexture[0], 0);

    // Set the targets for the fragment output variables
    glDrawBuffers(1, drawBuffers);
    ///////////////Velocity FBO #1: render to texture 1////////////////////
    // Bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, velocityFBO[1]);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, velocityTexture[1], 0);

    // Set the targets for the fragment output variables
    glDrawBuffers(1, drawBuffers);
    ///////////////////End////////////////////////
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
