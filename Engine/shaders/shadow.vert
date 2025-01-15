#version 450

layout(push_constant) uniform PushConstants {
    mat4 modelMatrix;         
    mat4 lightViewProj;       
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 attr1;
layout(location = 2) in vec3 attr2;
layout(location = 3) in vec3 attr3;

void main() {
    
    gl_Position = pc.lightViewProj * pc.modelMatrix * vec4(inPosition, 1.0);
}
