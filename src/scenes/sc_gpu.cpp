#include "../scenes.h"
#include "../engine/game.h"
#include "../engine/spatial.h"
#include "../engine/graphics.h"

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

    Pipeline *pipeline = nullptr;
    SDL_GPUBuffer *vertex_buffer = nullptr;
    SDL_GPUTransferBuffer *transfer_buffer = nullptr;

    UniformBuffer time_uniform{};
}

void SceneGpu::ready()
{
    // Create vertex buffer
    {
        SDL_GPUBufferCreateInfo buffer_info{};
        buffer_info.size = sizeof(vertices);
        buffer_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        vertex_buffer = SDL_CreateGPUBuffer(game->get_device(), &buffer_info);
    }

    // Create transfer buffer
    {
        SDL_GPUTransferBufferCreateInfo transfer_info{};
        transfer_info.size = sizeof(vertices);
        transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transfer_buffer = SDL_CreateGPUTransferBuffer(game->get_device(), &transfer_info);

        // Fill transfer buffer with data
        Vertex *data = (Vertex *)SDL_MapGPUTransferBuffer(game->get_device(), transfer_buffer, false);
        memcpy(data, vertices, sizeof(vertices));

        // Unmap the pointer when done updating the transfer buffer
        SDL_UnmapGPUTransferBuffer(game->get_device(), transfer_buffer);
    }

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
    std::string vertex_path = game->get_datapath() + "/shaders/vertex.spv";
    std::string fragment_path = game->get_datapath() + "/shaders/fragment.spv";
    
    Shader vertex_shader = Shader(game, Shader::Type::Vertex, vertex_path.c_str(), 0, 1, 0, 0);
    Shader fragment_shader = Shader(game, Shader::Type::Fragment, fragment_path.c_str(), 0, 1, 0, 0);

    // -- Setup pipeline --
    pipeline = new Pipeline(game, vertex_shader, fragment_shader);
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

    if (pipeline)
    {
        delete pipeline;
        pipeline = nullptr;
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
        pipeline->bind(render_pass);

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