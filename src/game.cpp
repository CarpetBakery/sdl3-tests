#include "game.h"
#include "time.h"

#include <atomic>
#include <iostream>

namespace
{
    constexpr float draw_fps = 60.0f;
    constexpr float game_fps = 60.0f;

    Uint64 game_time_last = 0;
    Uint64 game_time_accumulator = 0;
    int max_updates = 5; // Maximum updates to run before "giving up" and reducing frame rate

    // Audio
    constexpr int DEFAULT_SAMPLE_RATE = 44100;
    constexpr int AUDIO_BUFFER_SIZE_SAMPLES = 512;
    constexpr int AUDIO_CHANNELS = 2;
    constexpr int AUDIO_BUFFER_SIZE_BYTES = AUDIO_BUFFER_SIZE_SAMPLES * AUDIO_CHANNELS * sizeof(float);

    SDL_AudioStream *audio_stream = nullptr;
    float siner = 0.0f;
    
    std::atomic<float> pitch = 1.0f;

    float *audio_buffer = new float[AUDIO_BUFFER_SIZE_SAMPLES * AUDIO_CHANNELS];

    constexpr float base_freq = 110.0f;
    const float twelve_root_2 = SDL_pow(2.0f, 1.0f / 12.0f);

    bool audio_playing = false;
}

// Audio test
static void init_audio();
static void free_audio();
static void audio_callback(void *userdata, SDL_AudioStream *stream, int additionalAmount, int totalAmount);


bool Game::init(void *appstate, int width, int height)
{
    SDL_SetAppMetadata("First SDL3 Test", "1.0", "com.example.sdl3test");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }

    if (!SDL_CreateWindowAndRenderer("super mario 5", width, height, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return false;
    }
    SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // Init frame limiter
    game_time_last = Time::getTicks();
    game_time_accumulator = 0;

    init_audio();

    ready(appstate);

    return true;
}

void Game::ready(void *appstate)
{

}

void Game::tick(void *appstate)
{
    // -- Limit framerate --
    Uint64 timeTarget = (Uint64)((1.0 / draw_fps) * Time::ticksPerSecond);
    Uint64 timeCurr = Time::getTicks();
    Uint64 timeDiff = timeCurr - game_time_last;
    game_time_last = timeCurr;
    game_time_accumulator += timeDiff;

    // Don't let us run too fast
    while (game_time_accumulator < timeTarget)
    {
        int milliseconds = (int)(timeTarget - game_time_accumulator) / (Time::ticksPerSecond / 1000);
        if (milliseconds >= 0)
        {
            SDL_Delay((Uint32)milliseconds);
        }

        Uint64 timeCurr = Time::getTicks();
        Uint64 timeDiff = timeCurr - game_time_last;
        game_time_last = timeCurr;
        game_time_accumulator += timeDiff;
    }

    // Don't let us fall behind on too many updates
    Uint64 timeMax = max_updates * timeTarget;
    if (game_time_accumulator > timeMax)
    {
        game_time_accumulator = timeMax;
    }

    while (game_time_accumulator >= timeTarget)
    {
        game_time_accumulator -= timeTarget;

        Time::delta = (1.0f / draw_fps);
        Time::deltaTime = ((Time::delta * Time::ticksPerSecond) / (float)timeTarget / (draw_fps / game_fps));
        
        if (Time::pauseTimer > 0)
        {
            Time::pauseTimer -= Time::delta;
            if (Time::pauseTimer <= -0.0001)
            {
                Time::delta = -Time::pauseTimer;
            }
            else
            {
                continue;
            }
        }

        Time::previousTicks = Time::ticks;
        Time::ticks += timeTarget;
        Time::previousSeconds = Time::seconds;
        Time::seconds += Time::delta;

        update(appstate);
    }

    draw(appstate);
    SDL_RenderPresent(renderer);
}

void Game::quit(void *appstate, SDL_AppResult result)
{
    free_audio();
}

void Game::update(void *appstate)
{
    
}

void Game::draw(void *appstate)
{
    const int charsize = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDebugText(renderer, 272, 100, "Hello SDL3!");
    SDL_RenderDebugText(renderer, 224, 150, "Debug text and audio stream stuff");

    SDL_RenderDebugText(renderer, 124, 200, "Press 'SPACE' to play a sine wave. Up/Down to change pitch");
    SDL_RenderDebugTextFormat(renderer, 224, 225, "Pitch: %" SDL_PRIs32, (int)pitch);
    
    if (audio_playing)
    {
        SDL_RenderDebugText(renderer, 224, 250, "Playing!");
    }

    // For printf style substitutions
    SDL_RenderDebugTextFormat(renderer, ((float) (window_width() - (charsize * 46)) / 2), 400, "(This program has been running for %" SDL_PRIu64 " seconds.)", SDL_GetTicks() / 1000);
}

void Game::event(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN)
    {
        switch (event->key.key)
        {
        case SDLK_SPACE:
            if (audio_playing)
            {
                SDL_PauseAudioStreamDevice(audio_stream);
            }
            else
            {
                SDL_ResumeAudioStreamDevice(audio_stream);
            }
            audio_playing = !audio_playing;
            break;
        
        case SDLK_UP:
            pitch.store(pitch.load() + 1.0f);
            break;

        case SDLK_DOWN:
            pitch.store(pitch.load() - 1.0f);
            break;
        }
    }
}

void Game::destroy(void *appstate, SDL_AppResult result)
{

}

static void init_audio()
{
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
        float sample = SDL_sinf(siner * 2.0f * SDL_PI_F);
        for (int j = 0; j < AUDIO_CHANNELS; j++)
        {
            buf[i + j] = sample;
        }

        siner += (1.0f / (float)DEFAULT_SAMPLE_RATE) * 440.0f * SDL_powf(twelve_root_2, pitch.load());
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