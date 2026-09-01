#include "../scenes.h"

#include "../engine/game.h"
#include "../engine/time.h"
#include "../engine/math.h"

#include <SDL3/SDL.h>
#include <atomic>

namespace
{
    enum State
    {
        Main,
        EditSample,
        TheShapes
    };
    State state = State::EditSample;

    enum InstrumentMode
    {
        Sine,
        Custom
    };
    std::atomic<InstrumentMode> instrument_mode = InstrumentMode::Custom;

    bool program_startup = true;

    // Audio
    constexpr int DEFAULT_SAMPLE_RATE = 44100;
    constexpr int AUDIO_BUFFER_SIZE_SAMPLES = 512;
    constexpr int AUDIO_CHANNELS = 2;
    constexpr int AUDIO_BUFFER_SIZE_BYTES = AUDIO_BUFFER_SIZE_SAMPLES * AUDIO_CHANNELS * sizeof(float);

    SDL_AudioStream *audio_stream = nullptr;
    float siner = 0.0f;

    std::atomic<float> pitch = 1.0f;

    float *audio_buffer = nullptr;

    constexpr float base_freq = 110.0f;
    const float twelve_root_2 = Math::pow(2.0f, 1.0f / 12.0f);

    bool audio_playing = false;

    constexpr int key_count = 17;
    SDL_Keycode keys[key_count] = {
        SDLK_Z,
        SDLK_S,
        SDLK_X,
        SDLK_D,
        SDLK_C,
        SDLK_V,
        SDLK_G,
        SDLK_B,
        SDLK_H,
        SDLK_N,
        SDLK_J,
        SDLK_M,
        SDLK_COMMA,
        SDLK_L,
        SDLK_PERIOD,
        SDLK_SEMICOLON,
        SDLK_SLASH,
    };

    class SampleData
    {
    public:
        std::unique_ptr<float[]> data = nullptr;
        int data_size = 0;

        float get(float index)
        {
            return data[static_cast<int>(index)];
        }

        float get_safe(float index)
        {
            while (index < 0)
            {
                index += data_size;
            }
            while (index >= data_size)
            {
                index -= data_size;
            }
            return get(index);
        }

        void resize(int new_size)
        {
            if (data_size == new_size)
            {
                return;
            }

            std::unique_ptr<float[]> tmp = std::make_unique<float[]>(new_size);
            memset(tmp.get(), 0, new_size * sizeof(float));

            // Move old data
            if (data != nullptr && data_size > 0)
            {
                memcpy(tmp.get(), data.get(), Math::Min(data_size, new_size));
            }

            // Move everything over
            data_size = new_size;
            data = std::move(tmp);
        }

        // Fill data with a sine wave
        void fill_sine()
        {
            for (int i = 0; i < data_size; i++)
            {
                float fac = i / static_cast<float>(data_size);
                fac *= Math::pi * 2.0f;
                data[i] = Math::sin(fac) * 0.4f;
            }
        }
    };

    class Synth
    {
    public:
        float data_index = 0.0f;
        SampleData *sample_data = nullptr;

        void inc_data_index(int pitch)
        {
            // Increment index, wrap around
            float inc = Math::pow(twelve_root_2, pitch);

            data_index += inc;

            while (data_index > sample_data->data_size)
            {
                data_index -= sample_data->data_size;
            }
        }

        float eval_lagrange()
        {
            float sample;

            float sampleA, sampleB, sampleC, sampleD;
            float c0, c1, c2, c3;
            float margin = data_index - 2;
            float subPos = data_index - static_cast<int>(data_index);

            sampleA = sample_data->get_safe(margin - 1);
            sampleB = sample_data->get_safe(margin);
            sampleC = sample_data->get_safe(margin + 1);
            sampleD = sample_data->get_safe(margin + 2);

            c0 = sampleB;
            c1 = sampleC - 1 / 3.0f * sampleA - 1 / 2.0f * sampleB - 1 / 6.0f * sampleD;
            c2 = 1 / 2.0f * (sampleA + sampleC) - sampleB;
            c3 = 1 / 6.0f * (sampleD - sampleA) + 1 / 2.0f * (sampleB - sampleC);

            sample = ((c3 * subPos + c2) * subPos + c1) * subPos + c0;

            return sample;
        }
    };

    // -- Edit sample --
    constexpr int INIT_SAMPLE_COUNT = 128;
    Synth synth;
    SampleData sample_data;

    int edit_margin = 30;
    int edit_bottom_margin = 50;
    int edit_inner_margin = 15;

    Recti edit_outline;
    Recti edit_space;
    SDL_FRect edit_outline_rect;

    Vec2i mouse_pos_prev = Vec2i(0, 0);

    // -- The shapes --
    constexpr int RECT_COUNT = 20;
    Recti test_rects[RECT_COUNT];
}

// Audio test
static void init_audio(SceneAudio *scene);
static void free_audio();
static void audio_callback(void *userdata, SDL_AudioStream *stream, int additionalAmount, int totalAmount);

static void regenerate_rects(SceneAudio *scene);
static void musical_typing(SceneAudio *scene);

// -- States --
static void switch_state(SceneAudio *scene, State new_state);

static void main_update(SceneAudio *scene);
static void main_draw(SceneAudio *scene);

static void edit_sample_update(SceneAudio *scene);
static void edit_sample_draw(SceneAudio *scene);

static void the_shapes_update(SceneAudio *scene);
static void the_shapes_draw(SceneAudio *scene);

void SceneAudio::ready()
{
    init_audio(this);
}

void SceneAudio::update()
{
    switch (state)
    {
    case State::Main:
        main_update(this);
        break;
    case State::EditSample:
        edit_sample_update(this);
        break;
    case State::TheShapes:
        the_shapes_update(this);
        break;
    }
}

void SceneAudio::draw()
{
    const int charsize = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    SDL_Renderer *renderer = game->get_renderer();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    switch (state)
    {
    case State::Main:
        main_draw(this);
        break;
    case State::EditSample:
        edit_sample_draw(this);
        break;
    case State::TheShapes:
        the_shapes_draw(this);
        break;
    }
}

void SceneAudio::destroy()
{
    free_audio();
}

static void init_audio(SceneAudio *scene)
{
    audio_buffer = new float[AUDIO_BUFFER_SIZE_SAMPLES * AUDIO_CHANNELS];

    SDL_AudioSpec spec;
    spec.freq = DEFAULT_SAMPLE_RATE;
    spec.format = SDL_AUDIO_F32LE;
    spec.channels = AUDIO_CHANNELS;

    audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, audio_callback, nullptr);
    if (audio_stream == NULL)
    {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
        return;
    }

    if (audio_playing)
    {
        SDL_ResumeAudioStreamDevice(audio_stream);
    }

    // Sample edit
    edit_outline = Recti{
        edit_margin,
        edit_margin,
        scene->game->get_backbuffer_width() - edit_margin * 2,
        scene->game->get_backbuffer_height() - edit_margin * 2 - edit_bottom_margin};

    edit_space = Recti(
        edit_outline.x + edit_inner_margin,
        edit_outline.y + edit_inner_margin,
        edit_outline.w - edit_inner_margin * 2,
        edit_outline.h - edit_inner_margin * 2);

    edit_outline_rect = {
        static_cast<float>(edit_outline.x),
        static_cast<float>(edit_outline.y),
        static_cast<float>(edit_outline.w),
        static_cast<float>(edit_outline.h)};

    // Setup synth and sampledata
    sample_data.resize(INIT_SAMPLE_COUNT);
    synth.sample_data = &sample_data;

    if (program_startup)
    {
        program_startup = false;

        // Generate sin wave at startup
        sample_data.fill_sine();
    }

    // The shapes
    regenerate_rects(scene);
}

static void free_audio()
{
    if (audio_stream)
    {
        SDL_DestroyAudioStream(audio_stream);
    }

    delete[] audio_buffer;
}

static void audio_callback(void *userdata, SDL_AudioStream *stream, int additionalAmount, int totalAmount)
{
    int samples = totalAmount / sizeof(float);
    float *buf = new float[samples];
    memset(buf, 0, totalAmount);

    switch (instrument_mode.load())
    {
    case InstrumentMode::Sine:
        for (int i = 0; i < samples; i += 2)
        {
            float sample = Math::sin(siner * 2.0f * SDL_PI_F);
            for (int j = 0; j < AUDIO_CHANNELS; j++)
            {
                buf[i + j] = sample;
            }

            siner += (1.0f / (float)DEFAULT_SAMPLE_RATE) * 440.0f * Math::pow(twelve_root_2, pitch.load());
            while (siner > 1.0f)
            {
                siner -= 1.0f;
            }
        }
        break;

    case InstrumentMode::Custom:
        for (int i = 0; i < samples; i += 2)
        {
            // NOTE: This is SUPER thread unsafe. Editing sample data while audio thread
            // is reading it could be catastrophic
            float sample = synth.eval_lagrange();
            synth.inc_data_index(pitch.load());

            for (int j = 0; j < AUDIO_CHANNELS; j++)
            {
                buf[i + j] = sample;
            }
        }
        break;
    }

    // I think I might be using this wrong... not sure how to set the size of a buffer yet.
    // "totalAmount" seems to be over 3000 samples. Pretty big compared to what I normally use (512, 1024)
    SDL_PutAudioStreamData(stream, buf, totalAmount);
    delete[] buf;
}

static void switch_state(SceneAudio *scene, State new_state)
{
    switch (new_state)
    {
    case State::Main:
        break;
    case State::EditSample:
        mouse_pos_prev = scene->input->get_mouse_pos();
        break;
    case State::TheShapes:
        break;
    }

    state = new_state;
}

static void main_update(SceneAudio *scene)
{
    if (scene->input->key_pressed(SDLK_SPACE))
    {
        switch_state(scene, State::EditSample);
        if (audio_playing)
        {
            audio_playing = false;
            SDL_PauseAudioStreamDevice(audio_stream);
        }
        return;
    }

    musical_typing(scene);
}

static void main_draw(SceneAudio *scene)
{
    const int charsize = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;

    SDL_Renderer *renderer = scene->game->get_renderer();

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDebugText(renderer, 272, 100, "Hello SDL3!");
    SDL_RenderDebugText(renderer, 224, 150, "Debug text and audio stream stuff");

    SDL_RenderDebugText(renderer, 124, 200, "Use the bottom row of keys on the keyboard like a piano!");
    SDL_RenderDebugTextFormat(renderer, 224, 225, "Pitch: %" SDL_PRIs32, (int)pitch);

    auto mouse_pos = scene->input->get_mouse_pos();
    if (scene->input->mouse(MouseButton::Left))
    {
        SDL_RenderDebugText(renderer, mouse_pos.x, mouse_pos.y, "Morshu");
    }
    else
    {
        SDL_RenderDebugText(renderer, mouse_pos.x, mouse_pos.y, "Non-Morshu");
    }

    if (audio_playing)
    {
        SDL_RenderDebugText(renderer, 224, 250, "Playing!");
    }

    // For printf style substitutions
    SDL_RenderDebugTextFormat(renderer, ((float)(scene->game->get_backbuffer_width() - (charsize * 46)) / 2), 400, "(This program has been running for %" SDL_PRIu64 " seconds.)", SDL_GetTicks() / 1000);
}

static void edit_sample_update(SceneAudio *scene)
{
    if (scene->input->key_pressed(SDLK_SPACE))
    {
        switch_state(scene, State::Main);
    }

    if (scene->input->key_pressed(SDLK_R))
    {
        sample_data.fill_sine();
    }

    auto mouse_pos = scene->input->get_mouse_pos();

    // Draw the sample!
    if (scene->input->mouse(MouseButton::Left))
    {
        Linef mouse_line = Linef(mouse_pos_prev, mouse_pos);

        int sample_count = sample_data.data_size;
        float *custom_sample = sample_data.data.get();

        int w = Math::floor(edit_space.w / static_cast<float>(sample_count));
        for (int i = 0; i < sample_count; i++)
        {
            Recti r = Recti(
                edit_space.x + edit_space.w * (i / static_cast<float>(sample_count)),
                -edit_margin + edit_space.y,
                w,
                edit_margin * 2.0f + edit_space.h);

            if (mouse_line.intersects(r))
            {
                custom_sample[i] = (mouse_pos.y - edit_space.y - edit_space.h / 2.0f) / (edit_space.h / 2.0f);
                custom_sample[i] = Math::clampf(custom_sample[i], -1.0f, 1.0f);
            }
        }
    }

    musical_typing(scene);
    mouse_pos_prev = mouse_pos;
}

static void edit_sample_draw(SceneAudio *scene)
{
    SDL_Renderer *renderer = scene->game->get_renderer();

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDebugText(renderer, 10, 10, "-- EDIT SAMPLE --");

    SDL_RenderRect(renderer, &edit_outline_rect);

    int sample_count = sample_data.data_size;
    float *custom_sample = sample_data.data.get();

    // Draw samples
    Vec2i prev_point = Vec2i(0, 0);
    for (int i = 0; i < sample_count; i++)
    {
        float bip_width = 2.0f;
        SDL_FRect r = {
            edit_space.x + (edit_space.w - bip_width) * (i / static_cast<float>(sample_count)),
            edit_space.y + ((edit_space.h - bip_width) / 2.0f) + (((edit_space.h - bip_width) / 2.0f) * custom_sample[i]),
            bip_width,
            bip_width};

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderRect(renderer, &r);

        // Draw a line between samples
        if (i > 0)
        {
            SDL_SetRenderDrawColor(renderer, 0, 100, 120, SDL_ALPHA_OPAQUE);
            SDL_RenderLine(renderer, r.x, r.y, prev_point.x, prev_point.y);
        }
        prev_point = Vec2i(r.x, r.y);
    }
}

static void the_shapes_update(SceneAudio *scene)
{
    if (scene->input->key_pressed(SDLK_R))
    {
        regenerate_rects(scene);
    }

    int mag = 2;
    for (int i = 0; i < RECT_COUNT; i++)
    {
        Recti &r = test_rects[i];
        Vec2i move = Vec2i(Math::randRangei(-mag, mag),
                           Math::randRangei(-mag, mag));

        // Don't overlap other rectangles
        auto tmp = r;

        // Horizontal
        tmp.x += move.x;
        for (int j = 0; j < RECT_COUNT; j++)
        {
            Recti &check = test_rects[j];
            if (&r == &check)
            {
                continue;
            }

            if (check.overlaps(tmp))
            {
                tmp.x = r.x;
                break;
            }
        }

        // Vertical
        tmp.y += move.y;
        for (int j = 0; j < RECT_COUNT; j++)
        {
            Recti &check = test_rects[j];
            if (&r == &check)
            {
                continue;
            }

            if (check.overlaps(tmp))
            {
                tmp.y = r.y;
                break;
            }
        }

        r.x = tmp.x;
        r.y = tmp.y;

        // Don't exit the room
        r.x = Math::clampi(r.x, 0, scene->game->get_backbuffer_width() - r.w - 1);
        r.y = Math::clampi(r.y, 0, scene->game->get_backbuffer_height() - r.h - 1);
    }
}

static void the_shapes_draw(SceneAudio *scene)
{
    SDL_Renderer *renderer = scene->game->get_renderer();

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDebugText(renderer, 10, 10, "-- THE SHAPES --");

    auto mouse_pos = scene->input->get_mouse_pos();

    for (int i = 0; i < RECT_COUNT; i++)
    {
        // Draw rect in red if mouse is inside it
        if (test_rects[i].contains(mouse_pos))
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        }

        SDL_FRect r = {
            static_cast<float>(test_rects[i].x),
            static_cast<float>(test_rects[i].y),
            static_cast<float>(test_rects[i].w),
            static_cast<float>(test_rects[i].h)};

        SDL_RenderRect(renderer, &r);
    }
}

static void musical_typing(SceneAudio *scene)
{
    audio_playing = false;
    for (int i = 0; i < key_count; i++)
    {
        if (scene->game->input.key(keys[i]))
        {
            pitch.store(i);
            audio_playing = true;
        }
    }

    if (!audio_playing)
    {
        SDL_PauseAudioStreamDevice(audio_stream);
    }
    else
    {
        SDL_ResumeAudioStreamDevice(audio_stream);
    }
}

static void regenerate_rects(SceneAudio *scene)
{
    // Generate a bunch of random, non-overlapping rectangles
    for (int i = 0; i < RECT_COUNT; i++)
    {
        for (;;)
        {
            bool no_good = false;
            Vec2i s = Vec2i(
                Math::randRangei(16, 128),
                Math::randRangei(16, 128));

            Recti r = Recti(
                Vec2i(
                    Math::randRangei(0, scene->game->get_backbuffer_width() - s.x - 1),
                    Math::randRangei(0, scene->game->get_backbuffer_height() - s.y - 1)),
                s);

            for (int j = 0; j < i; j++)
            {
                if (test_rects[j].overlaps(r))
                {
                    no_good = true;
                    break;
                }
            }

            if (!no_good)
            {
                test_rects[i] = r;
                break;
            }
        }
    }
}