#version 430

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout(binding = 0) uniform sampler2D RenderTex;

uniform float EdgeThreshold;
uniform int Width;
uniform int Height;

subroutine vec4 RenderPassType();
subroutine uniform RenderPassType RenderPass;

struct LightInfo {
    vec4 Position;      // Light position in eye coords.
    vec3 Intensity;     // A, D, S intensity
};
uniform LightInfo Light;

struct MaterialInfo {
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Specular shininess factor
};
uniform MaterialInfo Material;

layout(location = 0) out vec4 FragColor;

vec3 phongModel(vec3 pos, vec3 norm) {
    vec3 s = normalize(vec3(Light.Position) - pos);
    vec3 v = normalize(vec3(-pos));
    vec3 h = normalize(v + s);
    return Light.Intensity * Material.Ka + Light.Intensity * Material.Kd * max(0.0, dot(s, norm)) +
        Light.Intensity * Material.Ks * pow(max(0.0, dot(h, norm)), Material.Shininess);
}

float luma(vec3 color) {
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

// Pass #1
subroutine (RenderPassType)
vec4 pass1() {
    return vec4(phongModel(Position, Normal), 1.0);
}

// Pass #2
subroutine (RenderPassType)
vec4 pass2() {
    float dx = 1.0 / float(Width);
    float dy = 1.0 / float(Height);
    float s00 = luma(texture(RenderTex, TexCoord + vec2(-dx, dy)).rgb);
    float s10 = luma(texture(RenderTex, TexCoord + vec2(-dx, 0.0)).rgb);
    float s20 = luma(texture(RenderTex, TexCoord + vec2(-dx, -dy)).rgb);
    float s01 = luma(texture(RenderTex, TexCoord + vec2(0.0, dy)).rgb);
    float s21 = luma(texture(RenderTex, TexCoord + vec2(0.0, -dy)).rgb);
    float s02 = luma(texture(RenderTex, TexCoord + vec2(dx, dy)).rgb);
    float s12 = luma(texture(RenderTex, TexCoord + vec2(dx, 0.0)).rgb);
    float s22 = luma(texture(RenderTex, TexCoord + vec2(dx, dy)).rgb);
    float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
    float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);
    float dist = sx * sx + sy * sy;
    if (dist > EdgeThreshold) {
        return vec4(1.0);
    }
    else {
        return vec4(0.0, 0.0, 0.0, 1.0);
    }
}

void main() {
    FragColor = RenderPass();
}
