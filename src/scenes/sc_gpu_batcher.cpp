#include "../scenes.h"
#include "../engine/game.h"
#include "../engine/graphics.h"

namespace
{
    struct SpriteInstance
    {
        float x, y, z;
        float rotation;
        float w, h;
        float padding_a, padding_b;
        float tex_u, tex_v, tex_w, tex_h;
        float r, g, b, a;
    };

    constexpr int sprite_count = 100;

    float u_coords[4] = {0.0f, 0.5f, 0.0f, 0.5f};
    float v_coords[4] = {0.0f, 0.0f, 0.5f, 0.5f};

    SDL_GPUGraphicsPipeline *sprite_pipeline = nullptr;
    SDL_GPUSampler *sampler = nullptr;
    SDL_GPUTexture *texture = nullptr;
    SDL_GPUTransferBuffer *sprite_data_transfer_buffer = nullptr;
    SDL_GPUBuffer *sprite_data_buffer = nullptr;
}

// https://moonside.games/posts/sdl-gpu-sprite-batcher/

void SceneGpuBatcher::ready()
{
    {
        // -- Compile shaders --
        std::string vertex_path = game->get_datapath() + "/shaders/vertex.spv";
        std::string fragment_path = game->get_datapath() + "/shaders/fragment.spv";

        Shader vertex_shader = Shader(game, Shader::Type::Vertex, vertex_path.c_str(), 0, 1, 1, 0);
        Shader fragment_shader = Shader(game, Shader::Type::Fragment, fragment_path.c_str(), 1, 0, 0, 0);

        // -- Setup pipeline --
        // Can't use my Pipeline class because it isn't flexible enough...
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
        
        target_info.num_color_targets = 0;
        target_info.color_target_descriptions = &color_target_info;
        // target_info.has_depth_stencil_target = false;

        pipeline_info.target_info = target_info;
        pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
        pipeline_info.vertex_shader = vertex_shader.gpu_shader();
        pipeline_info.fragment_shader = fragment_shader.gpu_shader();


        // NOTE: Could be failing because the hlsl shaders didn't get compiled correctly?
        // Or the directx to spirv shader compiler is mangling it somehow?
        // Might just use directX backend for this
        
        sprite_pipeline = SDL_CreateGPUGraphicsPipeline(game->get_device(), &pipeline_info);
    }

    // -- Load image data --
    {
        SDL_Surface *image_data = SDL_LoadPNG((game->get_datapath() + "/gfx/ravioli_atlas.png").c_str());
        if (image_data == NULL)
        {
            SDL_Log("Could not load image data.");
            return;
        }

        int image_data_size = image_data->w * image_data->h * 4;

        SDL_GPUTransferBufferCreateInfo image_transfer_info;
        image_transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        image_transfer_info.size = image_data_size;

        SDL_GPUTransferBuffer *texture_transfer_buffer = SDL_CreateGPUTransferBuffer(game->get_device(), &image_transfer_info);

        uint8_t *texture_transfer_ptr = (uint8_t*)SDL_MapGPUTransferBuffer(game->get_device(), texture_transfer_buffer, false);
        memcpy(texture_transfer_ptr, image_data->pixels, image_data_size);
        SDL_UnmapGPUTransferBuffer(game->get_device(), texture_transfer_buffer);

        // Create the GPU resources
        SDL_GPUTextureCreateInfo texture_create_info;
        texture_create_info.type = SDL_GPU_TEXTURETYPE_2D;
        texture_create_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        texture_create_info.width = image_data->w;
        texture_create_info.height = image_data->h;
        texture_create_info.layer_count_or_depth = 1;
        texture_create_info.num_levels = 1;
        texture_create_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;

        texture = SDL_CreateGPUTexture(game->get_device(), &texture_create_info);

        SDL_GPUSamplerCreateInfo sampler_info;
        sampler_info.min_filter = SDL_GPU_FILTER_NEAREST;
        sampler_info.mag_filter = SDL_GPU_FILTER_NEAREST;
        sampler_info.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
        sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

        sampler = SDL_CreateGPUSampler(game->get_device(), &sampler_info);

        SDL_GPUTransferBufferCreateInfo sprite_data_transfer_buffer_info;
        sprite_data_transfer_buffer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        sprite_data_transfer_buffer_info.size = sprite_count * sizeof(SpriteInstance);
        
        sprite_data_transfer_buffer = SDL_CreateGPUTransferBuffer(game->get_device(), &sprite_data_transfer_buffer_info);

        SDL_GPUBufferCreateInfo sprite_data_buffer_info;
        sprite_data_buffer_info.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
        sprite_data_buffer_info.size = sprite_count * sizeof(SpriteInstance);
        
        sprite_data_buffer = SDL_CreateGPUBuffer(game->get_device(), &sprite_data_buffer_info);

        // Transfer the data
        SDL_GPUCommandBuffer *upload_cmd_buf = SDL_AcquireGPUCommandBuffer(game->get_device());
        SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(upload_cmd_buf);

        SDL_GPUTextureTransferInfo texture_transfer_buffer_info;
        texture_transfer_buffer_info.transfer_buffer = texture_transfer_buffer;
        texture_transfer_buffer_info.offset = 0;

        SDL_GPUTextureRegion region;
        region.texture = texture;
        region.w = image_data->w;
        region.h = image_data->h;
        region.d = 1;

        SDL_UploadToGPUTexture(copy_pass, &texture_transfer_buffer_info, &region, false);

        SDL_EndGPUCopyPass(copy_pass);
        SDL_SubmitGPUCommandBuffer(upload_cmd_buf);

        SDL_DestroySurface(image_data);
        SDL_ReleaseGPUTransferBuffer(game->get_device(), texture_transfer_buffer);
    }
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
    Mat4x4 camera_matrix = Mat4x4::create_orthographic(0, 640, 480, 0, 0, -1);
    SDL_GPUCommandBuffer *cmd_buf = SDL_AcquireGPUCommandBuffer(game->get_device());

    if (cmd_buf == NULL)
    {
        SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        return;
    }

    SDL_GPUTexture *swapchain_tex;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buf, game->get_window(), &swapchain_tex, NULL, NULL))
    {
        SDL_Log("Acquire swapchain texture failed: %s", SDL_GetError());
        return;
    }

    if (swapchain_tex != NULL)
    {
        // Build sprite instance transfer
        SpriteInstance *data_ptr = (SpriteInstance *)SDL_MapGPUTransferBuffer(
            game->get_device(),
            sprite_data_transfer_buffer,
            true);

        for (int i = 0; i < sprite_count; i++)
        {
            int ravioli = rand() % 4;
            data_ptr[i].x = (float)(rand() % 640);
            data_ptr[i].y = (float)(rand() % 480);
            data_ptr[i].z = 0;
            data_ptr[i].rotation = 0;
            data_ptr[i].w = 32;
            data_ptr[i].h = 32;
            data_ptr[i].tex_u = u_coords[ravioli];
            data_ptr[i].tex_v = v_coords[ravioli];
            data_ptr[i].tex_w = 0.5f;
            data_ptr[i].tex_h = 0.5f;
            data_ptr[i].r = 1.0f;
            data_ptr[i].g = 1.0f;
            data_ptr[i].b = 1.0f;
            data_ptr[i].a = 1.0f;
        }
        SDL_UnmapGPUTransferBuffer(game->get_device(), sprite_data_transfer_buffer);

        // Upload sprite data
        {
            SDL_GPUTransferBufferLocation loc;
            loc.transfer_buffer = sprite_data_transfer_buffer;
            loc.offset = 0;

            SDL_GPUBufferRegion reg;
            reg.buffer = sprite_data_buffer;
            reg.offset = 0;
            reg.size = sprite_count * sizeof(SpriteInstance);

            SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(cmd_buf);
            SDL_UploadToGPUBuffer(copy_pass, &loc, &reg, true);
            SDL_EndGPUCopyPass(copy_pass);
        }

        // Render sprites
        {
            SDL_GPUColorTargetInfo color_target_info;
            color_target_info.texture = swapchain_tex;
            color_target_info.cycle = false;
            color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
            color_target_info.store_op = SDL_GPU_STOREOP_STORE;
            color_target_info.clear_color = {0, 0, 0, 1};

            SDL_GPUTextureSamplerBinding texture_sampler_binding;
            texture_sampler_binding.texture = texture;
            texture_sampler_binding.sampler = sampler;

            SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(cmd_buf, &color_target_info, 1, NULL);

            SDL_BindGPUGraphicsPipeline(render_pass, sprite_pipeline);
            SDL_BindGPUVertexStorageBuffers(render_pass, 0, &sprite_data_buffer, 1);
            SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_sampler_binding, 1);
            SDL_PushGPUVertexUniformData(cmd_buf, 0, &camera_matrix, sizeof(Mat4x4)); // NOTE: Might not work because it's my matrix struct
            SDL_DrawGPUPrimitives(render_pass, sprite_count * 6, 1, 0, 0);
            SDL_EndGPURenderPass(render_pass);
        }

        SDL_SubmitGPUCommandBuffer(cmd_buf);
    }
}
