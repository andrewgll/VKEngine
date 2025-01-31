#version 450
layout(location = 0) in vec3 position;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 lightViewProj;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;   
} push;

void main() {
    gl_Position = ubo.lightViewProj * push.modelMatrix * vec4(position, 1.0);
}
