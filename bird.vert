#version 430

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 TextureUV;
layout(location = 2) in vec3 VertexColor;
layout(location = 3) in float VertexNumber;
layout(location = 4) in vec2 TextureUV2;

layout(binding = 0, rgba16f) uniform image2D imagePosition;
layout(binding = 1, rgba16f) uniform image2D imageVelocity;

out vec4 Color;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

subroutine vec4 ColorSelector(vec3 velocity);
subroutine uniform ColorSelector selectColor;

// Calculate color by velocity at Z axis
subroutine(ColorSelector)
vec4 directionalColor(vec3 velocity) {
    return mix(vec4(1.0, 0.4, 0.1, 1.0), vec4(0.1, 0.4, 1.0, 1.0), (velocity.z + 1.0) / 2.0);
}

// Use default color
subroutine(ColorSelector)
vec4 randomColor(vec3 velocity) {
    return vec4(VertexColor, 1.0);
}

void main() {
    ivec2 uv = ivec2(TextureUV2.xy);
    vec4 tmpPosition = imageLoad(imagePosition, uv);
    vec3 pos = tmpPosition.xyz;
    vec3 velocity = normalize(imageLoad(imageVelocity, uv).xyz);

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

    Color = selectColor(velocity);
    //    if (uv.x > 16) {
    //        Color = vec4(1.0, 0.0, 0.0, 1.0);
    //    } else {
    //        Color = vec4(0.1, 1.0, 0.1, 1.0);
    //    }

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(newPosition, 1.0);
}