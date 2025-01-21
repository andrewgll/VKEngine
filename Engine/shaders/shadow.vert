#version 450
layout(location = 0) in vec3 position;


struct PointLight {
    vec4 position; // w is unused
    vec4 color;    // w is intensity
};

struct DirectionalLight {
    mat4 lightViewProj;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 invView;
    vec4 ambientLightColor; // w is intensity
    PointLight pointLights[10];
    DirectionalLight dirLight;
    int numLights;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;   
} push;

void main() {
    gl_Position = ubo.dirLight.lightViewProj * push.modelMatrix * vec4(position, 1.0);
}
