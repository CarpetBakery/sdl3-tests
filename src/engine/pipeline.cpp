#include "graphics.h"
#include "game.h"

namespace
{
    struct Vertex
    {
        float x, y, z;
        float r, g, b, a;
    };
}

// TODO: This is NOT a good abstraction of pipeline
Pipeline::Pipeline(Game *_game, const Shader &vertex_shader, const Shader &fragment_shader)
    : game(_game)
{
    SDL_GPUGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.vertex_shader = vertex_shader.gpu_shader();
    pipeline_info.fragment_shader = fragment_shader.gpu_shader();

    pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    // Describe the vertex buffers
    SDL_GPUVertexBufferDescription vertex_buffer_descriptions[1];
    vertex_buffer_descriptions[0].slot = 0;
    vertex_buffer_descriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertex_buffer_descriptions[0].instance_step_rate = 0;
    vertex_buffer_descriptions[0].pitch = sizeof(Vertex);

    pipeline_info.vertex_input_state.num_vertex_buffers = 1;
    pipeline_info.vertex_input_state.vertex_buffer_descriptions = vertex_buffer_descriptions;

    // Describe the vertex attribute
    SDL_GPUVertexAttribute vertex_attributes[2];

    // a_position
    vertex_attributes[0].buffer_slot = 0;                             // Fetch data from the buffer at slot 0
    vertex_attributes[0].location = 0;                                // Layout (location = 0) in shader
    vertex_attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3; // vec3
    vertex_attributes[0].offset = 0;                                  // Start from the first byte from current buffer position

    // a_color
    vertex_attributes[1].buffer_slot = 0;                             // Use buffer at slot 0
    vertex_attributes[1].location = 1;                                // Layout (location = 1) in shader
    vertex_attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4; // vec4
    vertex_attributes[1].offset = sizeof(float) * 3;                  // 4th float from current buffer position

    pipeline_info.vertex_input_state.num_vertex_attributes = 2;
    pipeline_info.vertex_input_state.vertex_attributes = vertex_attributes;

    // Describe the color target
    SDL_GPUColorTargetDescription color_target_descriptions[1];
    color_target_descriptions[0] = {};
    color_target_descriptions[0].format = SDL_GetGPUSwapchainTextureFormat(game->get_device(), game->get_window());

    pipeline_info.target_info.num_color_targets = 1;
    pipeline_info.target_info.color_target_descriptions = color_target_descriptions;

    // Build pipeline
    graphics_pipeline = SDL_CreateGPUGraphicsPipeline(game->get_device(), &pipeline_info);
}

Pipeline::~Pipeline()
{
    if (graphics_pipeline)
    {
        SDL_ReleaseGPUGraphicsPipeline(game->get_device(), graphics_pipeline);
        graphics_pipeline = nullptr;
    }
}

void Pipeline::bind(SDL_GPURenderPass *render_pass) const
{
    SDL_BindGPUGraphicsPipeline(render_pass, graphics_pipeline);
}