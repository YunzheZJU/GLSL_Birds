#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 2) in vec2 VertexCoord;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main() {
    TexCoord = VertexCoord;
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}