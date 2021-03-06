#version 430

uniform float base = 32;
uniform float delta = 0.016;
uniform float seperationDistance = 20;
uniform float alignmentDistance = 20;
uniform float cohesionDistance = 20;
uniform vec3 predator = vec3(1, 1, 1);

const float PI = 3.141592653589793;
const float PI_2 = PI * 2.0;

// 鸟与鸟碰撞的检测距离
float zoneRadius = 40.0;
// 平方即可，不开平方根加快计算
float zoneRadiusSquared = 1600.0;

float separationThresh = 0.45;
float alignmentThresh = 0.65;

const float BOUNDS = 400.0;

const float SPEED_LIMIT = 9.0;

layout(binding = 0, rgba32f) uniform image2D imageComputed;

layout(location = 0) out vec4 FragColor;

subroutine vec4 PositionGetter(ivec2 coord);
layout(location = 0) subroutine uniform PositionGetter getPosition;

subroutine vec4 VelocityGetter(ivec2 coord);
layout(location = 1) subroutine uniform VelocityGetter getVelocity;

layout(index = 0) subroutine(PositionGetter)
vec4 getUpperPosition(ivec2 coord) {
    return imageLoad(imageComputed, coord);
}

layout(index = 1) subroutine(PositionGetter)
vec4 getLowerPosition(ivec2 coord) {
    return imageLoad(imageComputed, ivec2(coord.x, coord.y + base));
}

layout(index = 2) subroutine(VelocityGetter)
vec4 getUpperVelocity(ivec2 coord) {
    return imageLoad(imageComputed, ivec2(coord.x + base, coord.y));
}

layout(index = 3) subroutine(VelocityGetter)
vec4 getLowerVelocity(ivec2 coord) {
    return imageLoad(imageComputed, ivec2(coord.x + base, coord.y + base));
}

subroutine void PositionSetter(ivec2 coord, vec4 data);
layout(location = 2) subroutine uniform PositionSetter setPosition;

subroutine void VelocitySetter(ivec2 coord, vec4 data);
layout(location = 3) subroutine uniform VelocitySetter setVelocity;

layout(index = 4) subroutine(PositionSetter)
void setUpperPosition(ivec2 coord, vec4 data) {
    imageStore(imageComputed, coord, data);
}

layout(index = 5) subroutine(PositionSetter)
void setLowerPosition(ivec2 coord, vec4 data) {
    imageStore(imageComputed, ivec2(coord.x, coord.y + base), data);
}

layout(index = 6) subroutine(VelocitySetter)
void setUpperVelocity(ivec2 coord, vec4 data) {
    imageStore(imageComputed, ivec2(coord.x + base, coord.y), data);
}

layout(index = 7) subroutine(VelocitySetter)
void setLowerVelocity(ivec2 coord, vec4 data) {
    imageStore(imageComputed, ivec2(coord.x + base, coord.y + base), data);
}

// Compute position
void position() {
    ivec2 uv = ivec2(gl_FragCoord.xy - 0.5);
    vec4 tmpPos = getPosition(uv);
    vec3 position = tmpPos.xyz;
    vec3 velocity = getVelocity(uv).xyz;

    float phase = tmpPos.w;

    // 和振翅有关，phase会循环变化，范围是0 - 20 * PI。恒定的phase导致不振翅，范围小于2 * PI的phase导致振翅缺帧
    phase = mod((phase + delta + length(velocity.xz) * delta * 3.0 + max(velocity.y, 0.0) * delta * 6.0), 62.83);

    // position + velocity即可，恒定帧率下delta无影响，15是系数
    setPosition(uv, vec4(position + velocity * delta * 15, phase));
}

// Compute velocity
void velocity() {
    // cohesionDistance再也没用到
    zoneRadius = seperationDistance + alignmentDistance + cohesionDistance;
    separationThresh = seperationDistance / zoneRadius;
    alignmentThresh = (seperationDistance + alignmentDistance) / zoneRadius;
    zoneRadiusSquared = zoneRadius * zoneRadius;

    // 当前鸟的信息在缓存纹理中的uv（所有鸟的信息被存在一张相应数量的像素数目的纹理中）
    ivec2 uv = ivec2(gl_FragCoord.xy);
    vec3 birdPosition, birdVelocity;

    // 自身的位置和速度
    vec3 selfPosition = getPosition(uv).xyz;
    vec3 selfVelocity = getVelocity(uv).xyz;

    float dist;
    vec3 dir; // direction
    float distSquared;

    float f;
    float percent;

    // 还是自身速度
    vec3 velocity = selfVelocity;

    float limit = SPEED_LIMIT;

    // predator从(-0.5, -0.5, 0.0)到(0.5， 0.5, 0.0)
    // dir是捕食者相对当前鸟的位置
    dir = predator * BOUNDS - selfPosition;
    dir.z = 0.;
    // 与捕食者的距离
    dist = length(dir);
    distSquared = dist * dist;

    // 寻找捕食者的距离
    float preyRadius = 100.0;
    float preyRadiusSq = preyRadius * preyRadius;

    // 与捕食者距离过远不会引发躲避行为
    if (dist < preyRadius) {
        // move birds away from predator
        // 躲避速度的大小
        f = (distSquared / preyRadiusSq - 1.0) * delta * 100.;
        // 躲避方向为远离捕食者方向
        velocity += normalize(dir) * f;
        // 增加速度上限
        limit += 5.0;
    }

    // 向中心的吸引力
    // Attract flocks to the center
    vec3 central = vec3(0., 0., 0.);
    dir = selfPosition - central;
    dist = length(dir);

    // 调整中心吸引的方向，在高度上出现摆动
    dir.y *= 2.5;
    velocity -= normalize(dir) * delta * 5.;

    // 对两个缓存纹理（全体鸟的位置和速度）遍历
    for (float y = 0.0; y < base; y++) {
        for (float x = 0.0; x < base; x++) {
            // 为啥要加0.5？不加0.5也能跑
            // 因为在取纹理的时候就是有0.5
            ivec2 ref = ivec2(x, y);
            birdPosition = getPosition(ref).xyz;

            dir = birdPosition - selfPosition;
            dist = length(dir);

            // 同一只鸟
            if (dist < 0.0001) continue;

            distSquared = dist * dist;

            // 距离过远
            if (distSquared > zoneRadiusSquared) continue;

            // 与最大距离相比
            percent = distSquared / zoneRadiusSquared;

            if (percent < separationThresh) { // low
                // 低于separation阈值，靠得太近了
                // Separation - Move apart for comfort
                f = (separationThresh / percent - 1.0) * delta;
                velocity -= normalize(dir) * f;
            } else if (percent < alignmentThresh) { // high
                // 低于alignment阈值（separation的值也有影响）
                // Alignment - fly the same direction
                float threshDelta = alignmentThresh - separationThresh;
                float adjustedPercent = (percent - separationThresh) / threshDelta;
                // 取出参照鸟的速度，把参照鸟的速度方向加到自己身上
                birdVelocity = getVelocity(ref).xyz;
                f = (0.5 - cos(adjustedPercent * PI_2) * 0.5 + 0.5) * delta;
                velocity += normalize(birdVelocity) * f;
            } else {
                // 否则：靠得有点远
                // Attraction / Cohesion - move closer
                float threshDelta = 1.0 - alignmentThresh;
                float adjustedPercent = (percent - alignmentThresh) / threshDelta;

                f = (0.5 - (cos(adjustedPercent * PI_2) * -0.5 + 0.5)) * delta;

                velocity += normalize(dir) * f;
            }
        }
    }

    // This make tends to fly around than down or up
    if (velocity.y > 0.0) velocity.y *= (1.0 - 0.2 * delta);

    // Speed Limits
    if (length(velocity) > limit) {
        velocity = normalize(velocity) * limit;
    }

    setVelocity(uv, vec4(velocity, 1.0));
}

void main() {
	position();
	velocity();
	FragColor = vec4(1.0);
}
