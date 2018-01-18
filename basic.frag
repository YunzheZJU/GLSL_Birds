#version 430

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout(binding = 0) uniform sampler2D RenderTex;


layout(location = 0) out vec4 FragColor;


void main() {
    FragColor = texture(RenderTex, TexCoord);
}
