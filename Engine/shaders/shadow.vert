#version 450

// Push constants structure matching your C++ implementation
layout(push_constant) uniform PushConstants {
    mat4 modelMatrix;         // Model matrix for the current object
    mat4 lightViewProj;       // Combined light's view and projection matrix
} pc;

// Input vertex attributes
layout(location = 0) in vec3 inPosition;

// Main shader entry point
void main() {
    // Transform the vertex position to light space
    gl_Position = pc.lightViewProj * pc.modelMatrix * vec4(inPosition, 1.0);
}
