#version 460

layout (location = 0) in vec4 v_color;
layout (location = 0) out vec4 FragColor;

// NOTE: SDL uses set 1 for uniforms in VERTEX shaders
// NOTE: std140 is memory layout. Careful of weird padding on complex uniforms
// https://wiki.libsdl.org/SDL3/SDL_CreateGPUShader
layout (std140, set = 3, binding = 0) uniform UniformBlock {
    float time;
};

void main()
{
    // float pulse = sin(time * 2.0) * 0.5 + 0.5; // Range [0, 1]
    // FragColor = vec4(v_color.rgb * (0.8 + pulse * 0.5), v_color.a);
    
    FragColor = v_color;
}
