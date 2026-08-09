#include "scenes.h"
#include "engine/game.h"
#include "engine/spatial.h"
#include "engine/graphics.h"

#include <SDL3/SDL.h>
#include <filesystem>

namespace
{
    // Using my own types... slower, im sure
    // struct Vertex
    // {
    //     Vec3f position;
    //     Color color;
    // };

    struct Vertex
    {
        float x, y, z;
        float r, g, b, a;
    };

    struct UniformBuffer
    {
        float time;
    };

    static Vertex vertices[] = {
        {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},   // top vertex
        {-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f}, // bottom left vertex
        {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f}   // bottom right vertex
    };

    SDL_GPUShader *vertex_shader = nullptr;
    SDL_GPUShader *fragment_shader = nullptr;

    SDL_GPUGraphicsPipeline *graphics_pipeline = nullptr;
    SDL_GPUBuffer *vertex_buffer = nullptr;
    SDL_GPUTransferBuffer *transfer_buffer = nullptr;

    UniformBuffer time_uniform{};
}

void SceneGpu::ready()
{
    // Create vertex buffer
    SDL_GPUBufferCreateInfo buffer_info{};
    buffer_info.size = sizeof(vertices); // Is "sizeof" gonna work here???
    buffer_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertex_buffer = SDL_CreateGPUBuffer(game->get_device(), &buffer_info);

    // Create transfer buffer
    SDL_GPUTransferBufferCreateInfo transfer_info{};
    transfer_info.size = sizeof(vertices);
    transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transfer_buffer = SDL_CreateGPUTransferBuffer(game->get_device(), &transfer_info);

    // Fill transfer buffer with data
    Vertex *data = (Vertex *)SDL_MapGPUTransferBuffer(game->get_device(), transfer_buffer, false);

    memcpy(data, vertices, sizeof(vertices));

    // Unmap the pointer when done updating the transfer buffer
    SDL_UnmapGPUTransferBuffer(game->get_device(), transfer_buffer);

    // -- Transfer data from transfer buffer to vertex buffer --
    {
        // Start a copy pass
        SDL_GPUCommandBuffer *cmd_buf = SDL_AcquireGPUCommandBuffer(game->get_device());
        SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(cmd_buf);

        // Where's the data?
        SDL_GPUTransferBufferLocation location{};
        location.transfer_buffer = transfer_buffer;
        location.offset = 0;

        SDL_GPUBufferRegion region{};
        region.buffer = vertex_buffer;
        region.size = sizeof(vertices);
        region.offset = 0; // Begin writing from the first vertex

        // Upload data
        SDL_UploadToGPUBuffer(copy_pass, &location, &region, true);

        // End the copy pass
        SDL_EndGPUCopyPass(copy_pass);
        SDL_SubmitGPUCommandBuffer(cmd_buf);
    }

    // -- Compile shaders --
    {
        std::string vertex_path = game->get_datapath() + "/shaders/vertex.spv";
        std::string fragment_path = game->get_datapath() + "/shaders/fragment.spv";

        LB_ASSERT(std::filesystem::exists(vertex_path), "Vertex shader not found.");
        LB_ASSERT(std::filesystem::exists(fragment_path), "Fragment shader not found.");

        // Vertex shader
        size_t vertex_code_size;
        void *vertex_code = SDL_LoadFile(vertex_path.c_str(), &vertex_code_size);

        // Create the vertex shader
        SDL_GPUShaderCreateInfo vertex_info{};
        vertex_info.code = (Uint8 *)vertex_code; // Convert to array of bytes
        vertex_info.code_size = vertex_code_size;
        vertex_info.entrypoint = "main";
        vertex_info.format = SDL_GPU_SHADERFORMAT_SPIRV; // For loading .spv shaders
        vertex_info.stage = SDL_GPU_SHADERSTAGE_VERTEX;

        // No textures, storage buffers, or uniforms (yet)
        // I have to keep track of this per-shader...!?
        // UPDATE: SDL_shadercross apparently has shader reflection that can fill this out
        vertex_info.num_samplers = 0;
        vertex_info.num_storage_buffers = 0;
        vertex_info.num_storage_textures = 0;
        vertex_info.num_uniform_buffers = 1;

        vertex_shader = SDL_CreateGPUShader(game->get_device(), &vertex_info);
        SDL_free(vertex_code);

        // Fragment shader
        size_t fragment_code_size;
        void *fragment_code = SDL_LoadFile(fragment_path.c_str(), &fragment_code_size);

        // Create the fragment shader

        SDL_GPUShaderCreateInfo fragment_info{};
        fragment_info.code = (Uint8 *)fragment_code;
        fragment_info.code_size = fragment_code_size;
        fragment_info.entrypoint = "main";
        fragment_info.format = SDL_GPU_SHADERFORMAT_SPIRV;
        fragment_info.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;

        fragment_info.num_samplers = 0;
        fragment_info.num_storage_buffers = 0;
        fragment_info.num_storage_textures = 0;
        fragment_info.num_uniform_buffers = 1;

        fragment_shader = SDL_CreateGPUShader(game->get_device(), &fragment_info);
        SDL_free(fragment_code);
    }

    // -- Setup pipeline --
    SDL_GPUGraphicsPipelineCreateInfo pipeline_info{};

    // Bind shaders
    pipeline_info.vertex_shader = vertex_shader;
    pipeline_info.fragment_shader = fragment_shader;

    // Draw triangles
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

    // Example blending setup
    // color_target_descriptions[0].blend_state.enable_blend = true;
    // color_target_descriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    // color_target_descriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    // color_target_descriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    // color_target_descriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    // color_target_descriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    // color_target_descriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    pipeline_info.target_info.num_color_targets = 1;
    pipeline_info.target_info.color_target_descriptions = color_target_descriptions;

    // Build pipeline, release shaders
    graphics_pipeline = SDL_CreateGPUGraphicsPipeline(game->get_device(), &pipeline_info);
    SDL_ReleaseGPUShader(game->get_device(), vertex_shader);
    SDL_ReleaseGPUShader(game->get_device(), fragment_shader);
}

void SceneGpu::destroy()
{
    if (vertex_buffer)
    {
        SDL_ReleaseGPUBuffer(game->get_device(), vertex_buffer);
        vertex_buffer = nullptr;
    }

    if (transfer_buffer)
    {
        SDL_ReleaseGPUTransferBuffer(game->get_device(), transfer_buffer);
        transfer_buffer = nullptr;
    }

    if (graphics_pipeline)
    {
        SDL_ReleaseGPUGraphicsPipeline(game->get_device(), graphics_pipeline);
        graphics_pipeline = nullptr;
    }
}

void SceneGpu::update()
{
}

void SceneGpu::draw()
{
    SDL_GPUCommandBuffer *cmd_buf = SDL_AcquireGPUCommandBuffer(game->get_device());
    if (cmd_buf == NULL)
    {
        SDL_Log("Couldn't get command buffer: %s", SDL_GetError());
        // return SDL_APP_FAILURE;
    }

    SDL_GPUTexture *swapchain_texture;

    // If not used correctly, this can lead to memory leaks on certain backends
    // https://hamdy-elzanqali.medium.com/let-there-be-triangles-sdl-gpu-edition-bd82cf2ef615
    // SDL_AcquireGPUSwapchainTexture(cmd_buf, game->get_window(), &swapchain_texture, NULL, NULL);

    // Waits for vsync?
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buf, game->get_window(), &swapchain_texture, NULL, NULL))
    {
        SDL_Log("Couldn't get swapchain texture: %s", SDL_GetError());
        // return SDL_APP_FAILURE;
    }

    // NOTE: swapchain_texture can sometimes be null like when the window is minimized
    if (swapchain_texture != NULL)
    {
        // Create a color target
        SDL_GPUColorTargetInfo color_target_info{};
        color_target_info.texture = swapchain_texture;
        color_target_info.cycle = true;
        color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        color_target_info.store_op = SDL_GPU_STOREOP_STORE;
        color_target_info.clear_color = {0.16f, 0.47f, 0.34f, 1.0f};

        // Begin a render pass
        SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(cmd_buf, &color_target_info, 1, NULL);

        // Bind pipeline
        SDL_BindGPUGraphicsPipeline(render_pass, graphics_pipeline);

        // Bind vertex buffer
        SDL_GPUBufferBinding buffer_bindings[1];
        buffer_bindings[0].buffer = vertex_buffer;
        buffer_bindings[0].offset = 0;
        SDL_BindGPUVertexBuffers(render_pass, 0, buffer_bindings, 1); // Bind one buffer starting from slot 0

        // Set uniforms
        time_uniform.time = SDL_GetTicksNS() / 1e9f; // Time since startup in seconds
        SDL_PushGPUVertexUniformData(cmd_buf, 0, &time_uniform, sizeof(UniformBuffer));
        SDL_PushGPUFragmentUniformData(cmd_buf, 0, &time_uniform, sizeof(UniformBuffer));

        // Issue a draw call
        SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);

        SDL_EndGPURenderPass(render_pass);
    }

    // Submit command buffer for drawing
    SDL_SubmitGPUCommandBuffer(cmd_buf);
}