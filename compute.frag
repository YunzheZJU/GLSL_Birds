#version 400

uniform float time;
uniform float delta; // about 0.016
uniform float seperationDistance = 20; // 20
uniform float alignmentDistance = 40; // 40
uniform float cohesionDistance = 20; //
uniform float freedomFactor;
uniform vec3 predator;

const vec2 resolution = vec2(1280.0, 720.0);
const float width = resolution.x;
const float height = resolution.y;

const float PI = 3.141592653589793;
const float PI_2 = PI * 2.0;

// 鸟与鸟碰撞的检测距离
float zoneRadius = 40.0;
// 平方即可，不开平方根加快计算
float zoneRadiusSquared = 1600.0;

float separationThresh = 0.45;
float alignmentThresh = 0.65;

const float BOUNDS = 400.0;
const float UPPER_BOUNDS = BOUNDS;
const float LOWER_BOUNDS = -UPPER_BOUNDS;

const float SPEED_LIMIT = 9.0;

layout(location = 0) out vec4 FragColor;

subroutine vec4 ComputePassType();
subroutine uniform ComputePassType ComputerPass;

// Compute position
subroutine(ComputePassType)
vec4 position() {
    return vec4(1.0);
}

// Compute velocity
subroutine(ComputePassType)
vec4 velocity() {
    return vec4(1.0);
}

void main() {
	FragColor = ComputerPass();

}
