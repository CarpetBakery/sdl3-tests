#include "graphics.h"

class GraphicsDeviceSdl : public GraphicsDevice
{
public:
    GraphicsDeviceSdl(Game *_game) : GraphicsDevice(_game) {}

    void create() override
    {
    }

    void destroy() override
    {
    }

    void present() override
    {
    }

    ResourceHandle create_texture(const std::string &name, int width, int height, int format, int flags) override
    {
    }

    void set_texture_data(ResourceHandle texture, void *data, int data_size, const Recti &dest) override
    {
    }

    void get_texture_data(ResourceHandle texture, void *data, int data_size, const Recti &src) override
    {
    }

    void blit_texture(ResourceHandle source_texture, const Recti &src, ResourceHandle dest_texture, const Recti &dst) override
    {
    }

    ResourceHandle create_target(int width, int height) override
    {
    }

    ResourceHandle create_shader(Shader &shader) override
    {
    }

    ResourceHandle create_buffer(const std::string &name, BufferType type, IndexFormat format) override
    {
    }

    void upload_buffer_data(ResourceHandle buffer, void *data, int data_size, int data_dest_offset) override
    {
    }

    void destroy_resource(ResourceHandle resource) override
    {
    }

    void perform_draw(const DrawCommand &command) override
    {
    }

    void clear(const Color &color, float depth) override
    {
    }
};

GraphicsDevice *GraphicsDevice::make_sdl(Game *_game)
{
    return new GraphicsDeviceSdl(_game);
}