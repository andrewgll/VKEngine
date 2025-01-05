#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler;

struct PointLight {
  vec4 position;
  vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 invView;
    vec4 ambientLightColor; // w is intensity
    PointLight pointLights[10]; // update this number in the engine for the max number of lights
    int numLights;
} ubo;


layout(push_constant) uniform Push { 
    mat4 modelMatrix; // projectiuon * view * model
    mat4 normalMatrix;
} push;

void main(){
    vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 specularLight = vec3(0.0);
    vec3 surfaceNormal = normalize(fragNormalWorld);   

    vec3 cameraPosWorld = ubo.invView[3].xyz; // camera position in world space   
    vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld); // direction from fragment to camera

    for (int i = 0; i < ubo.numLights; i++) {
        PointLight light = ubo.pointLights[i];
        vec3 directionToLight = light.position.xyz - fragPosWorld;
        float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared

        directionToLight = normalize(directionToLight);

        float cosAngIncidence =max(dot(surfaceNormal, directionToLight), 0);

        vec3 intensity = light.color.xyz * light.color.w * attenuation;

        diffuseLight += intensity * cosAngIncidence;

        //specular light
        vec3 halfVector = normalize(viewDirection + directionToLight);
        float blinnTerm = dot(halfVector, surfaceNormal);
        blinnTerm = clamp(blinnTerm, 0, 1);

        blinnTerm = pow(blinnTerm, 100.0); // shininess

        specularLight += intensity * blinnTerm;

    }

    vec3 imageColor = texture(texSampler, fragTexCoord).xyz;

    outColor = vec4((diffuseLight * fragColor +  specularLight * fragColor)*imageColor, 1.0);
    // outColor = vec4(fragTexCoord, 1.0,1.0);
}