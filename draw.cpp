//
// Draw.cpp
// Drawing components of the scene
// Created by Yunzhe on 2017/12/4.
//

#include "draw.h"

GLuint vaoHandle;

void DrawScene() {
    glutSolidTeapot(1);
//    glBindVertexArray(vaoHandle);
//    // 从0号顶点数组中每取三个画一个三角形。
//    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawLocator(GLfloat *center, GLfloat radius) {
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glLineWidth(1);
    glTranslatef(center[X], center[Y], center[Z]);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 20; i++) {
        glVertex3f(radius * cos(2 * PI / 20 * i), radius * sin(2 * PI / 20 * i), 0);
    }
    glEnd();
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 20; i++) {
        glVertex3f(radius * cos(2 * PI / 20 * i), 0, radius * sin(2 * PI / 20 * i));
    }
    glEnd();
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 20; i++) {
        glVertex3f(0, radius * sin(2 * PI / 20 * i), radius * cos(2 * PI / 20 * i));
    }
    glEnd();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawCrosshair() {
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
    glColor3f(0.1f, 1.0f, 0.1f);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(-11.0f, 0.0f);
    glVertex2f(10.0f, 0.0f);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0.0f, -10.0f);
    glVertex2f(0.0f, 10.0f);
    glEnd();
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);            // 选择投影矩阵
    glPopMatrix();                            // 重置为原保存矩阵
    glMatrixMode(GL_MODELVIEW);                // 选择Modelview矩阵
    glPopMatrix();                            // 重置为原保存矩阵
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}
