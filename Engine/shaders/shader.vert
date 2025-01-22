#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor; 
layout(location = 1) out vec3 fragPosWorld; 
layout(location = 2) out vec3 fragNormalWorld; 
layout(location = 3) out vec2 fragUv;

layout(location = 4) out vec4 lightSpacePos;

struct PointLight {
  vec4 position;
  vec4 color;
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
    mat4 normalMatrix;
    int hasNormalMap;
} push;


void main() {
    vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
    gl_Position = ubo.projection * ubo.view * positionWorld; 

    fragNormalWorld = normalize(mat3(push.modelMatrix) * normal);
    fragPosWorld = positionWorld.xyz;
    fragColor = color;
    fragUv = uv;
    lightSpacePos = ubo.dirLight.lightViewProj * push.modelMatrix * vec4(position, 1.0);
} 