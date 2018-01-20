#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 TextureUV;
layout(location = 2) in vec3 VertexColor;
layout(location = 3) in float VertexNumber;
layout(location = 4) in vec2 TextureUV2;

layout(binding = 0, rgba32f) uniform image2D imageComputed;

out vec4 Color;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform float base = 32;

subroutine vec4 ColorSelector(vec3 velocity);
layout(location = 0) subroutine uniform ColorSelector selectColor;

subroutine vec4 PositionGetter(ivec2 coord);
layout(location = 1) subroutine uniform PositionGetter getPosition;

subroutine vec4 VelocityGetter(ivec2 coord);
layout(location = 2) subroutine uniform VelocityGetter getVelocity;

// Calculate color by velocity at Z axis
layout(index = 0) subroutine(ColorSelector)
vec4 directionalColor(vec3 velocity) {
    return mix(vec4(1.0, 0.4, 0.1, 1.0), vec4(0.1, 0.4, 1.0, 1.0), (velocity.z + 1.0) / 2.0);
}

// Use default color
layout(index = 1) subroutine(ColorSelector)
vec4 randomColor(vec3 velocity) {
    return vec4(VertexColor, 1.0);
}

layout(index = 2) subroutine(PositionGetter)
vec4 getUpperPosition(ivec2 coord) {
    return imageLoad(imageComputed, coord);
}

layout(index = 3) subroutine(PositionGetter)
vec4 getLowerPosition(ivec2 coord) {
    return imageLoad(imageComputed, ivec2(coord.x, coord.y + base));
}

layout(index = 4) subroutine(VelocityGetter)
vec4 getUpperVelocity(ivec2 coord) {
    return imageLoad(imageComputed, ivec2(coord.x + base, coord.y));
}

layout(index = 5) subroutine(VelocityGetter)
vec4 getLowerVelocity(ivec2 coord) {
    return imageLoad(imageComputed, ivec2(coord.x + base, coord.y + base));
}

void main() {
    ivec2 uv = ivec2(TextureUV2.xy);
    vec4 tmpPosition = getPosition(uv);
    vec3 pos = tmpPosition.xyz;
    vec3 velocity = normalize(getVelocity(uv).xyz);

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

    newPosition += pos;

    Color = selectColor(velocity);

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(newPosition, 1.0);
}