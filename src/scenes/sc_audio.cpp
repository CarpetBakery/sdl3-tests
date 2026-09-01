#include "../scenes.h"

#include "../engine/game.h"
#include "../engine/time.h"
#include "../engine/math.h"

#include <SDL3/SDL.h>
#include <atomic>

namespace
{
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
}

// Audio test
static void init_audio();
static void free_audio();
static void audio_callback(void *userdata, SDL_AudioStream *stream, int additionalAmount, int totalAmount);

void SceneAudio::ready()
{
    init_audio();
}

void SceneAudio::update()
{
    // if (game->input.key_pressed(SDLK_SPACE))
    // {
    //     if (audio_playing)
    //     {
    //         SDL_PauseAudioStreamDevice(audio_stream);
    //     }
    //     else
    //     {
    //         SDL_ResumeAudioStreamDevice(audio_stream);
    //     }
    //     audio_playing = !audio_playing;
    // }
    
    // if (game->input.key_pressed(SDLK_UP))
    // {
    //     pitch.store(pitch.load() + 1.0f);
    // }

    // if (game->input.key_pressed(SDLK_DOWN))
    // {
    //     pitch.store(pitch.load() - 1.0f);
    // }

    audio_playing = false;
    for (int i = 0; i < key_count; i++)
    {
        if (game->input.key(keys[i]))
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

void SceneAudio::draw()
{
    const int charsize = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    SDL_Renderer *renderer = game->get_renderer();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDebugText(renderer, 272, 100, "Hello SDL3!");
    SDL_RenderDebugText(renderer, 224, 150, "Debug text and audio stream stuff");

    SDL_RenderDebugText(renderer, 124, 200, "Use the bottom row of keys on the keyboard like a piano!");
    SDL_RenderDebugTextFormat(renderer, 224, 225, "Pitch: %" SDL_PRIs32, (int)pitch);

    auto mouse_pos = input->get_mouse_pos();
    if (input->mouse(MouseButton::Left))
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
    SDL_RenderDebugTextFormat(renderer, ((float) (game->get_backbuffer_width() - (charsize * 46)) / 2), 400, "(This program has been running for %" SDL_PRIu64 " seconds.)", SDL_GetTicks() / 1000);
}

void SceneAudio::destroy()
{
    free_audio();
}

static void init_audio()
{
    audio_buffer = new float[AUDIO_BUFFER_SIZE_SAMPLES * AUDIO_CHANNELS];
    
    SDL_AudioSpec spec;
    spec.freq = DEFAULT_SAMPLE_RATE;
    spec.format = SDL_AUDIO_F32LE;
    spec.channels = AUDIO_CHANNELS;
    printf("Working\n");

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

    // I think I might be using this wrong... not sure how to set the size of a buffer yet.
    // "totalAmount" seems to be over 3000 samples. Pretty big compared to what I normally use (512, 1024)
    SDL_PutAudioStreamData(stream, buf, totalAmount);

    delete[] buf;
}