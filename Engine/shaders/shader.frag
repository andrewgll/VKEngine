#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec2 fragUv;

layout(set = 0, binding = 1) uniform sampler2D shadowMap; 

layout(set = 1, binding = 1) uniform sampler2D albedoTexture;
layout(set = 1, binding = 2) uniform sampler2D normalTexture;
layout(set = 1, binding = 3) uniform sampler2D roughnessTexture;
layout(set = 1, binding = 4) uniform sampler2D metallicTexture;
layout(set = 1, binding = 5) uniform sampler2D aoTexture;

layout(location = 0) out vec4 outColor;

const float PI = 3.14159265359;

struct PointLight {
    vec4 position; // w is unused
    vec4 color;    // w is intensity
};
struct DirectionalLight {
    vec3 direction; // Direction towards the light source
    vec3 color;     // RGB color of the light
    float intensity; // Intensity of the light
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 invView;
    vec4 ambientLightColor;
    PointLight pointLights[10];
    DirectionalLight dirLight;
    mat4 lightViewProj; 
    int numLights;
} ubo;


layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
    bool hasNormalMap;
} push;

vec3 getNormal() {
    vec3 normal = fragNormalWorld;
    if (push.hasNormalMap) {
        vec3 tangentNormal = texture(normalTexture, fragUv).rgb * 2.0 - 1.0;
        vec3 T = normalize(mat3(push.normalMatrix) * vec3(1.0, 0.0, 0.0));
        vec3 B = normalize(mat3(push.normalMatrix) * vec3(0.0, 1.0, 0.0));
        vec3 N = normalize(mat3(push.normalMatrix) * fragNormalWorld);
        mat3 TBN = mat3(T, B, N);
        normal = normalize(TBN * tangentNormal);
    }
    return normalize(normal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float denom = NdotV * (1.0 - k) + k;
    return NdotV / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
float calculateShadow(vec4 fragPosLightSpace) {
    
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    
    projCoords = projCoords * 0.5 + 0.5;

    
    if (projCoords.z > 1.0) return 0.0;

    
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    
    float bias = max(0.05 * (1.0 - dot(fragNormalWorld, ubo.dirLight.direction)), 0.005);
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main() {
    vec3 albedo = pow(texture(albedoTexture, fragUv).rgb, vec3(2.2)); // sRGB to linear
    float metallic = texture(metallicTexture, fragUv).r;
    float roughness = clamp(texture(roughnessTexture, fragUv).r, 0.05, 1.0); // Avoid zero roughness
    float ao = texture(aoTexture, fragUv).r;

    vec3 N = getNormal();
    vec3 V = normalize(ubo.invView[3].xyz - fragPosWorld);

    vec3 F0 = vec3(0.04); // Default dielectric reflectance
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < ubo.numLights; ++i) {
        PointLight light = ubo.pointLights[i];
        vec3 L = normalize(light.position.xyz - fragPosWorld);
        vec3 H = normalize(V + L);
        float distance = length(light.position.xyz - fragPosWorld);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light.color.rgb * light.color.a * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 nominator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // Prevent divide by zero
        vec3 specular = nominator / denominator;

        // kS is the specular reflectance, kD is diffuse reflectance
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

     // Calculate Directional Light Contribution
    vec3 L_dir = normalize(-ubo.dirLight.direction); // Light direction (towards light)
    float shadow = calculateShadow(ubo.lightViewProj * vec4(fragPosWorld, 1.0)); 
    shadow = 1.0 - shadow;
    vec3 H_dir = normalize(V + L_dir);
    vec3 radiance_dir = ubo.dirLight.color * ubo.dirLight.intensity;

    float NDF_dir = DistributionGGX(N, H_dir, roughness);
    float G_dir = GeometrySmith(N, V, L_dir, roughness);
    vec3 F_dir = fresnelSchlick(max(dot(H_dir, V), 0.0), F0);

    vec3 nominator_dir = NDF_dir * G_dir * F_dir;
    float denominator_dir = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L_dir), 0.0) + 0.001;
    vec3 specular_dir = nominator_dir / denominator_dir;

    vec3 kS_dir = F_dir;
    vec3 kD_dir = vec3(1.0) - kS_dir;
    kD_dir *= 1.0 - metallic;

    float NdotL_dir = max(dot(N, L_dir), 0.0);
    Lo += shadow*(kD_dir * albedo / PI + specular_dir) * radiance_dir * NdotL_dir;
    Lo = clamp(Lo, vec3(0.0), vec3(10.0)); 

    // Ambient Lighting
    vec3 ambient = (ubo.ambientLightColor.rgb * ubo.ambientLightColor.a) * albedo * ao;
    vec3 color = ambient * ao + Lo; 

    // Gamma Correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    outColor = vec4(color,  1);

}
