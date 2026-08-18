#include "graphics.h"
#include "debug.h"
#include "game.h"

#include <filesystem>

Shader::Shader(Game *_game, Type _type,
               const char *code_path,
               int sampler_count,
               int uniform_buffer_count,
               int storage_buffer_count,
               int storage_texture_count)
    : game(_game), type(_type)
{
    LB_ASSERT(std::filesystem::exists(code_path), "Fragment shader not found.");

    // Vertex shader
    size_t code_size;
    void *code = SDL_LoadFile(code_path, &code_size);

    // Create the shader
    // TODO: Figure out how to get counts by parsing file
    SDL_GPUShaderCreateInfo info{};
    info.code = (Uint8 *)code; // Convert to array of bytes
    info.code_size = code_size;
    info.entrypoint = "main";
    info.format = SDL_GPU_SHADERFORMAT_SPIRV; // For loading .spv shaders

    switch (type)
    {
    case Type::Vertex:
        info.stage = SDL_GPU_SHADERSTAGE_VERTEX;
        break;
    case Type::Fragment:
        info.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
        break;
    case Type::Compute:
        // TODO
        LB_ASSERT(false, "Compute shaders not implemented yet.");
        break;
    }

    // SDL_shadercross apparently has shader reflection that can fill this out
    info.num_samplers = sampler_count;
    info.num_storage_buffers = storage_buffer_count;
    info.num_storage_textures = storage_texture_count;
    info.num_uniform_buffers = uniform_buffer_count;

    shader = SDL_CreateGPUShader(game->get_device(), &info);
    SDL_free(code);
}

Shader::~Shader()
{
    LB_ASSERT(game, "Reference to game not found.");

    // Free program memory
    if (shader)
    {
        SDL_ReleaseGPUShader(game->get_device(), shader);
        shader = nullptr;
    }
}
