//
// Created by Yunzhe on 2018/1/16.
//

#include "vbobird.h"

VBOBird::VBOBird(int base) {
    int nBirds = base * base;           // Total of birds
    nVerts = nBirds * 9;                // 9 vertices per bird
    auto *v = new float[3 * nVerts];
    auto *uv = new float[2 * nVerts];
    auto *c = new float[3 * nVerts];
    auto *n = new float[nVerts];
    auto *el = new GLuint[nVerts];

    for (int i = 0; i < nBirds; i++) {
        // Body
        v[i * 27] = 0;
        v[i * 27 + 1] = 0;
        v[i * 27 + 2] = -4.0f;

        v[i * 27 + 3] = 0;
        v[i * 27 + 4] = 0.8f;
        v[i * 27 + 5] = -4.0f;

        v[i * 27 + 6] = 0;
        v[i * 27 + 7] = 0;
        v[i * 27 + 8] = 6.0f;

        // Left Wing
        v[i * 27 + 9] = 0;
        v[i * 27 + 10] = 0;
        v[i * 27 + 11] = -3.0f;

        v[i * 27 + 12] = -4.0f;
        v[i * 27 + 13] = 0;
        v[i * 27 + 14] = 0;

        v[i * 27 + 15] = 0;
        v[i * 27 + 16] = 0;
        v[i * 27 + 17] = 3.0f;

        // Right Wing
        v[i * 27 + 18] = 0;
        v[i * 27 + 19] = 0;
        v[i * 27 + 20] = 3.0f;

        v[i * 27 + 21] = 4.0f;
        v[i * 27 + 22] = 0;
        v[i * 27 + 23] = 0;

        v[i * 27 + 24] = 0;
        v[i * 27 + 25] = 0;
        v[i * 27 + 26] = -3.0f;
    }

    srand(static_cast<unsigned int>(time(nullptr)));

//    for (int i = 0; i < nVerts * 3; i++) {
//        v[i] += (i / 27) / 10.0;
//    }

    for (int i = 0; i < nVerts; i++) {
        int f = i / 9;  // Should be floor, and why f = i / 3 ? No sense.
        auto x = static_cast<float>((f % base) * 1.0 / base);
        auto y = static_cast<float>((int) (f * 1.0 / base) * 1.0 / base);

        uv[i * 2] = x;
        uv[i * 2 + 1] = y;

        c[i * 3] = static_cast<float>((rand() % 100 / 100.0) * 0.8 + 0.1);
        c[i * 3 + 1] = static_cast<float>((rand() % 100 / 100.0) * 0.8 + 0.1);
        c[i * 3 + 2] = static_cast<float>((rand() % 100 / 100.0) * 0.8 + 0.1);

        n[i] = i % 9;

        el[i] = static_cast<GLuint>(i);
    }

    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    unsigned int handle[5];
    glGenBuffers(5, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, nVerts * 3 * sizeof(float), v, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint) 0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) nullptr + (0)));
    glEnableVertexAttribArray(0);  // VertexPosition

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, nVerts * 2 * sizeof(float), uv, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint) 1, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) nullptr + (0)));
    glEnableVertexAttribArray(1);  // TextureUV

    glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
    glBufferData(GL_ARRAY_BUFFER, nVerts * 3 * sizeof(float), c, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint) 2, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) nullptr + (0)));
    glEnableVertexAttribArray(2);  // VertexColor

    glBindBuffer(GL_ARRAY_BUFFER, handle[3]);
    glBufferData(GL_ARRAY_BUFFER, nVerts * sizeof(float), n, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint) 3, 1, GL_FLOAT, GL_FALSE, 0, ((GLubyte *) nullptr + (0)));
    glEnableVertexAttribArray(3);  // VertexNumber

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nVerts * sizeof(GLuint), el, GL_STATIC_DRAW);

    glBindVertexArray(0);

}

void VBOBird::render() const {
    glBindVertexArray(vaoHandle);
    glDrawElements(GL_TRIANGLES, nVerts, GL_UNSIGNED_INT, ((GLubyte *) nullptr + (0)));

}
