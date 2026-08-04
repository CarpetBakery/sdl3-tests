#include "scenes.h"
#include "engine/game.h"
#include "engine/spatial.h"
#include "engine/graphics.h"

#include <SDL3/SDL.h>


namespace
{
    struct Vertex
    {
        Vec3f position;
        Color color;
    };
}

void SceneGpu::ready()
{}

void SceneGpu::update()
{}

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
        color_target_info.clear_color =  {0.16f, 0.47f, 0.34f, 1.0f};

        // Begin a render pass
        SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(cmd_buf, &color_target_info, 1, NULL);

        // Draw something

        SDL_EndGPURenderPass(render_pass);
    }

    // Submit command buffer for drawing
    SDL_SubmitGPUCommandBuffer(cmd_buf);
}

void SceneGpu::destroy()
{}
