//
// Created by Yunzhe on 2018/1/16.
//

#include "vbobird.h"

VBOBird::VBOBird(int number) {
    float v[9 * 3] = {
            // Body
            0, 0, -2.0f,
            0, 0.4f, -2.0f,
            0, 0, 3.0f,
            // Left Wing
            0, 0, -1.5f,
            -2.0f, 0, 0,
            0, 0, 1.5f,
            // Right Wing
            0, 0, 1.5f,
            2.0f, 0, 0,
            0, 0, -1.5f
    };

    float uv[9 * 2] = {
            0, 0,
            0, 0,
            0, 0,

            0.5, 0,
            0.5, 0,
            0.5, 0,

            0, 0.5,
            0, 0.5,
            0, 0.5
    };

    float color[9 * 3] = {
            1, 0, 0,
            1, 0, 0,
            1, 0, 0,

            0, 1, 0,
            0, 1, 0,
            0, 1, 0,

            0, 0, 1,
            0, 0, 1,
            0, 0, 1,
    };

    float n[9] = {
            0, 1, 2,
            3, 4, 5,
            6, 7, 8
    };

    GLuint el[9] = {
            0, 1, 2,
            3, 4, 5,
            6, 7, 8
    };

    glGenVertexArrays( 1, &vaoHandle );
    glBindVertexArray(vaoHandle);

    unsigned int handle[5];
    glGenBuffers(5, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 9 * 3 * sizeof(float), v, GL_STATIC_DRAW);
    glVertexAttribPointer( (GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(0);  // VertexPosition

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 9 * 2 * sizeof(float), uv, GL_STATIC_DRAW);
    glVertexAttribPointer( (GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(1);  // TextureUV

    glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
    glBufferData(GL_ARRAY_BUFFER, 9 * 3 * sizeof(float), color, GL_STATIC_DRAW);
    glVertexAttribPointer( (GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(2);  // VertexColor

    glBindBuffer(GL_ARRAY_BUFFER, handle[3]);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), n, GL_STATIC_DRAW);
    glVertexAttribPointer( (GLuint)3, 1, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)) );
    glEnableVertexAttribArray(3);  // texture coords

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 9 * sizeof(GLuint), el, GL_STATIC_DRAW);

    glBindVertexArray(0);

}

void VBOBird::render() const {
    glBindVertexArray(vaoHandle);
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, ((GLubyte *)NULL + (0)));

}
