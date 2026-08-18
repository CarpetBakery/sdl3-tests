#pragma once
#include "spatial.h"

#include <SDL3/SDL.h>
#include <string>

using ResourceHandle = int;

class Game;
class Shader;
class Pipeline;
class GraphicsDevice;
class Color;

enum RendererType
{
    Sdl,
    Gl,
    Gpu
};

enum GraphicsDriver
{
    None,
    Vulkan,
    D3d12,
    Metal,
    Web,
};

enum IndexFormat
{
    Sixteen,
    ThirtyTwo,
};



struct DrawCommand
{
    // TODO
};

class Shader
{
public:
    enum Type
    {
        Vertex,
        Fragment,
        Compute,
    };
    
private:
    Game *game = nullptr;
    SDL_GPUShader *shader = nullptr;
    Type type;

public:
    unsigned int ID;

    Shader() = delete;
    Shader(Game *_game, Type _type,
           const char *code_path,
           int sampler_count,
           int uniform_buffer_count,
           int storage_buffer_count,
           int storage_texture_count);

    ~Shader();

    Shader(const Shader &) = delete;
    Shader(Shader &&) = delete;
    Shader &operator=(const Shader &) = delete;
    Shader &operator=(Shader &&) = delete;

    inline SDL_GPUShader *gpu_shader() const
    {
        return shader;
    }

    inline Type get_type() const
    {
        return type;
    }
};

class Pipeline
{
private:
    Game *game = nullptr;
    SDL_GPUGraphicsPipeline *graphics_pipeline = nullptr;

public:
    Pipeline() = delete;
    Pipeline(Game *_game, const Shader &vertex_shader, const Shader &fragment_shader);
    ~Pipeline();

    Pipeline(const Pipeline &) = delete;
    Pipeline(Pipeline &&) = delete;
    Pipeline &operator=(const Pipeline &) = delete;
    Pipeline &operator=(Pipeline &&) = delete;

    void bind(SDL_GPURenderPass *render_pass) const;
};

class GraphicsDevice
{
public:
    enum BufferType
    {
        Vertex,
        Index,
        Storage,
        Compute,
    };

protected:
    Game *game = nullptr;
    GraphicsDriver driver;

public:
    GraphicsDevice(Game *_game) : game(_game) {}
    virtual ~GraphicsDevice() = default;

    GraphicsDevice(const GraphicsDevice &) = delete;
    GraphicsDevice(GraphicsDevice &&) = delete;
    GraphicsDevice &operator=(const GraphicsDevice &) = delete;
    GraphicsDevice &operator=(GraphicsDevice &&) = delete;

    virtual void create() = 0;
    virtual void destroy() = 0;
    // virtual void window_created() = 0;
    // virtual void window_destroyed() = 0;
    virtual void present() = 0;

    // TODO: Need resource management
    virtual ResourceHandle create_texture(const std::string &name, int width, int height, int format, int flags) = 0;
    virtual void set_texture_data(ResourceHandle texture, void *data, int data_size, const Recti &dest) = 0;
    virtual void get_texture_data(ResourceHandle texture, void *data, int data_size, const Recti &src) = 0;
    virtual void blit_texture(ResourceHandle source_texture, const Recti &src, ResourceHandle dest_texture, const Recti &dst) = 0;
    virtual ResourceHandle create_target(int width, int height) = 0;
    virtual ResourceHandle create_shader(Shader &shader) = 0;
    virtual ResourceHandle create_buffer(const std::string &name, BufferType type, IndexFormat format) = 0;
    virtual void upload_buffer_data(ResourceHandle buffer, void *data, int data_size, int data_dest_offset) = 0;
    virtual void destroy_resource(ResourceHandle resource) = 0;
    virtual void perform_draw(const DrawCommand &command) = 0;
    virtual void clear(const Color &color, float depth) = 0;

    static GraphicsDevice *make_sdl(Game *_game);
    // static GraphicsDevice *make_gl(Game *_game); // There is going to be a lot of pain
};

class Color
{
public:
    uint8_t r, g, b, a;

    constexpr Color()
        : r(0), g(0), b(0), a(255) {}

    constexpr Color(uint8_t r, uint8_t g, uint8_t b)
        : r(r), g(g), b(b), a(255) {}

    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        : r(r), g(g), b(b), a(a) {}

    constexpr static Color fromFloat(float r, float g, float b, float a = 1.0f)
    {
        Color color;
        color.r = static_cast<uint8_t>(r * 255.0f);
        color.g = static_cast<uint8_t>(g * 255.0f);
        color.b = static_cast<uint8_t>(b * 255.0f);
        color.a = static_cast<uint8_t>(a * 255.0f);
        return color;
    }

    constexpr Color lerp(const Color &rhs, float fac) const
    {
        return Color(
            Math::lerp(r, rhs.r, fac),
            Math::lerp(g, rhs.g, fac),
            Math::lerp(b, rhs.b, fac),
            Math::lerp(a, rhs.a, fac));
    }

    constexpr float r_f32() const
    {
        return static_cast<float>(r) / 255.0f;
    }
    constexpr float g_f32() const
    {
        return static_cast<float>(g) / 255.0f;
    }
    constexpr float b_f32() const
    {
        return static_cast<float>(b) / 255.0f;
    }
    constexpr float a_f32() const
    {
        return static_cast<float>(a) / 255.0f;
    }

    // Doesn't multiply alpha
    constexpr Color operator*(float f) const
    {
        return Color(
            r * f,
            g * f,
            b * f,
            a);
    }

    // Doesn't multiply alpha
    constexpr Color &operator*=(const float rhs)
    {
        r *= rhs;
        g *= rhs;
        b *= rhs;
        return *this;
    }

    constexpr bool operator==(const Color &rhs) const
    {
        return (
            r == rhs.r &&
            g == rhs.g &&
            b == rhs.b &&
            a == rhs.a);
    }

    constexpr bool operator!=(const Color &rhs) const
    {
        return !(*this == rhs);
    }

    std::string to_string() const
    {
        return "(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a) + ")";
    }

    static const Color white;
    static const Color black;
    static const Color red;
    static const Color green;
    static const Color blue;
    static const Color vsBlue;
};

inline const Color Color::white = Color(255, 255, 255);
inline const Color Color::black = Color(0, 0, 0);
inline const Color Color::red = Color(255, 0, 0);
inline const Color Color::green = Color(0, 255, 0);
inline const Color Color::blue = Color(0, 0, 255);
inline const Color Color::vsBlue = Color(93, 107, 153);