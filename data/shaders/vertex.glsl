#version 460

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;
layout (location = 0) out vec4 v_color;

layout (std140, set = 1, binding = 0) uniform UniformBlock {
    float time;
};

void main()
{
    // vec3 new_pos = a_position;
    // new_pos.y += sin(time * 3.5) * 0.075;
    
    // gl_Position = vec4(new_pos, 1.0f);
    // v_color = a_color;

    gl_Position = vec4(a_position, 1.0);
    v_color = a_color;
}
