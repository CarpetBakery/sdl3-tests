#include "../scenes.h"
#include "../engine/game.h"
#include "../engine/graphics.h"

namespace
{
    struct Vertex
    {
        float x, y, z;
        float r, g, b, a;
    };

    struct UniformBuffer
    {
        float time;
    };

    SDL_GPUGraphicsPipeline *sprite_pipeline = nullptr;
}

// https://moonside.games/posts/sdl-gpu-sprite-batcher/

void SceneGpuBatcher::ready()
{
    // -- Compile shaders --
    std::string vertex_path = game->get_datapath() + "/shaders/vertex.spv";
    std::string fragment_path = game->get_datapath() + "/shaders/fragment.spv";
    Shader shader = Shader(game,
                           vertex_path.c_str(), 0, 1, 1, 0,
                           fragment_path.c_str(), 1, 0, 0, 0);

    // -- Setup pipeline --
    SDL_GPUGraphicsPipelineCreateInfo pipeline_info;
    SDL_GPUGraphicsPipelineTargetInfo target_info;
    SDL_GPUColorTargetDescription color_target_info;
    SDL_GPUColorTargetBlendState blend_state;

    blend_state.enable_blend = true;
    blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    
    color_target_info.format = SDL_GetGPUSwapchainTextureFormat(game->get_device(), game->get_window());
    color_target_info.blend_state = blend_state;
    
    target_info.num_color_targets = 1;
    target_info.color_target_descriptions = &color_target_info;

    pipeline_info.target_info = target_info;
    pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipeline_info.vertex_shader = shader.get_vertex();
    pipeline_info.fragment_shader = shader.get_fragment();
    
    sprite_pipeline = SDL_CreateGPUGraphicsPipeline(game->get_device(), &pipeline_info);
}

void SceneGpuBatcher::destroy()
{
    if (sprite_pipeline)
    {
        SDL_ReleaseGPUGraphicsPipeline(game->get_device(), sprite_pipeline);
        sprite_pipeline = nullptr;
    }
}

void SceneGpuBatcher::update()
{
}

void SceneGpuBatcher::draw()
{

}
