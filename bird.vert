#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 TextureUV;
layout(location = 2) in vec3 VertexColor;
layout(location = 3) in float VertexNumber;

layout(binding = 0) uniform sampler2D texturePosition;
layout(binding = 1) uniform sampler2D textureVelocity;

out vec4 Color;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform mat4 ModelViewMatrix;
uniform mat4 MVP;

void main() {
    vec4 tmpPosition = texture2D(texturePosition, TextureUV);
    vec3 pos = tmpPosition.xyz;
    vec3 velocity = normalize(texture2D(textureVelocity, TextureUV).xyz);

    vec3 newPosition = VertexPosition;

    if (VertexNumber == 4.0 || VertexNumber == 7.0) {
        // 振翅
        newPosition.y = sin(tmpPosition.w) * 5;
    }

    newPosition = mat3(ModelMatrix) * newPosition;

    velocity.z *= -1.;
    float xz = length(velocity.xz);
    float xyz = 1.;
    float x = sqrt(1. - velocity.y * velocity.y);

    float cosry = velocity.x / xz;
    float sinry = velocity.z / xz;

    float cosrz = x / xyz;
    float sinrz = velocity.y / xyz;

    mat3 maty = mat3(
        cosry,  0,  -sinry,
        0,      1,  0,
        sinry,  0,  cosry
    );

    mat3 matz = mat3(
        cosrz,  sinrz,  0,
        -sinrz, cosrz,  0,
        0,      0,      1
    );

    newPosition = maty * matz * newPosition;
    if (length(pos) < 0.4) {
        pos = vec3(100.5);
    }
    newPosition += pos;

    Color = vec4(VertexColor, 1.0);
//    if (TextureUV.y == 0 && TextureUV.x == 0) {
//        Color = vec4(1.0, 0.0, 0.0, 1.0);
//    }

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(newPosition, 1.0);
//    gl_Position = MVP * vec4(VertexPosition + pos, 1.0);
}