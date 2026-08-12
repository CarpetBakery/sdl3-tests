#include "graphics.h"
#include "debug.h"
#include "game.h"

#include <filesystem>

Shader::Shader(Game *_game, const char *vertex_path, const char *fragment_path)
    : game(_game)
{
    LB_ASSERT(std::filesystem::exists(vertex_path), "Vertex shader not found.");
    LB_ASSERT(std::filesystem::exists(fragment_path), "Fragment shader not found.");

    // Vertex shader
    size_t vertex_code_size;
    void *vertex_code = SDL_LoadFile(vertex_path, &vertex_code_size);

    // NOTE: This is flawed because we don't have a way of knowing haw many uniform buffers
    // there are yet... Apparently there's a way to know this by parsing the file differently
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
    void *fragment_code = SDL_LoadFile(fragment_path, &fragment_code_size);

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

Shader::~Shader()
{
    LB_ASSERT(game, "Reference to game not found.");
    
    // Free program memory
    if (vertex_shader)
    {
        SDL_ReleaseGPUShader(game->get_device(), vertex_shader);
        vertex_shader = nullptr;
    }
    
    if (fragment_shader)
    {
        SDL_ReleaseGPUShader(game->get_device(), fragment_shader);
        fragment_shader = nullptr;
    }
}
