#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 attr1;
layout(location = 2) in vec3 attr2;
layout(location = 3) in vec3 attr3;

layout(push_constant) uniform Push {
    mat4 lightViewProj;       
} push;

void main() {

    gl_Position = push.lightViewProj  * vec4(inPosition, 1.0);
}
