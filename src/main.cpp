#define SDL_MAIN_USE_CALLBACKS 1 // Use callbacks instead of the old overriding main method
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <atomic>

#include "time.h"

namespace
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    constexpr int WINDOW_WIDTH = 640;
    constexpr int WINDOW_HEIGHT = 480;

    constexpr float drawFps = 60.0f;
    constexpr float gameFps = 60.0f;

    Uint64 gameTimeLast = 0;
    Uint64 gameTimeAccumulator = 0;
    int maxUpdates = 5; // Maximum updates to run before "giving up" and reducing frame rate

    // Audio
    constexpr int DEFAULT_SAMPLE_RATE = 44100;
    constexpr int AUDIO_BUFFER_SIZE_SAMPLES = 512;
    constexpr int AUDIO_CHANNELS = 2;
    constexpr int AUDIO_BUFFER_SIZE_BYTES = AUDIO_BUFFER_SIZE_SAMPLES * AUDIO_CHANNELS * sizeof(float);

    SDL_AudioStream *audioStream = nullptr;
    float siner = 0.0f;
    
    std::atomic<float> pitch = 1.0f;

    float *audioBuffer = new float[AUDIO_BUFFER_SIZE_SAMPLES * AUDIO_CHANNELS];

    constexpr float baseFreq = 110.0f;
    const float twelveRoot2 = SDL_pow(2.0f, 1.0f / 12.0f);

    bool audioPlaying = false;
}

void update(void *appstate);
void draw(void *appstate);

// Audio test
void initAudio();
void freeAudio();
void audioCallback(void *userdata, SDL_AudioStream *stream, int additionalAmount, int totalAmount);


// Startup function
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("First SDL3 Test", "1.0", "com.example.sdl3test");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("super mario 5", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // Init frame limiter
    gameTimeLast = Time::getTicks();
    gameTimeAccumulator = 0;

    initAudio();

    return SDL_APP_CONTINUE;
}

// Runs when a new event occurs
// Replaces iterating over events every frame(?)
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_KEY_DOWN)
    {
        switch (event->key.key)
        {
        case SDLK_SPACE:
            if (audioPlaying)
            {
                SDL_PauseAudioStreamDevice(audioStream);
            }
            else
            {
                SDL_ResumeAudioStreamDevice(audioStream);
            }
            audioPlaying = !audioPlaying;
            break;
        
        case SDLK_UP:
            pitch.store(pitch.load() + 1.0f);
            break;

        case SDLK_DOWN:
            pitch.store(pitch.load() - 1.0f);
            break;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    // -- Limit framerate --
    Uint64 timeTarget = (Uint64)((1.0 / drawFps) * Time::ticksPerSecond);
    Uint64 timeCurr = Time::getTicks();
    Uint64 timeDiff = timeCurr - gameTimeLast;
    gameTimeLast = timeCurr;
    gameTimeAccumulator += timeDiff;

    // Don't let us run too fast
    while (gameTimeAccumulator < timeTarget)
    {
        int milliseconds = (int)(timeTarget - gameTimeAccumulator) / (Time::ticksPerSecond / 1000);
        if (milliseconds >= 0)
        {
            SDL_Delay((Uint32)milliseconds);
        }

        Uint64 timeCurr = Time::getTicks();
        Uint64 timeDiff = timeCurr - gameTimeLast;
        gameTimeLast = timeCurr;
        gameTimeAccumulator += timeDiff;
    }

    // Don't let us fall behind on too many updates
    Uint64 timeMax = maxUpdates * timeTarget;
    if (gameTimeAccumulator > timeMax)
    {
        gameTimeAccumulator = timeMax;
    }

    while (gameTimeAccumulator >= timeTarget)
    {
        gameTimeAccumulator -= timeTarget;

        Time::delta = (1.0f / drawFps);
        Time::deltaTime = ((Time::delta * Time::ticksPerSecond) / (float)timeTarget / (drawFps / gameFps));
        
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

    return SDL_APP_CONTINUE;
}

// Function runs at shutdown
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // SDL will clean up window/renderer for us???
    freeAudio();
}

void update(void *appstate)
{}

void draw(void *appstate)
{
    const int charsize = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDebugText(renderer, 272, 100, "Hello SDL3!");
    SDL_RenderDebugText(renderer, 224, 150, "Debug text and audio stream stuff");

    SDL_RenderDebugText(renderer, 124, 200, "Press 'SPACE' to play a sine wave. Up/Down to change pitch");
    SDL_RenderDebugTextFormat(renderer, 224, 225, "Pitch: %" SDL_PRIs32, (int)pitch);
    
    if (audioPlaying)
    {
        SDL_RenderDebugText(renderer, 224, 250, "Playing!");
    }

    // For printf style substitutions
    SDL_RenderDebugTextFormat(renderer, ((float) (WINDOW_WIDTH - (charsize * 46)) / 2), 400, "(This program has been running for %" SDL_PRIu64 " seconds.)", SDL_GetTicks() / 1000);
}


void initAudio()
{
    SDL_AudioSpec spec;
    spec.freq = DEFAULT_SAMPLE_RATE;
    spec.format = SDL_AUDIO_F32LE;
    spec.channels = AUDIO_CHANNELS;
    printf("Working\n");

    audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, audioCallback, nullptr);
    if (audioStream == NULL)
    {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
        return;
    }

    if (audioPlaying)
    {
        SDL_ResumeAudioStreamDevice(audioStream);
    }
}

void freeAudio()
{
    if (audioStream)
    {
        SDL_DestroyAudioStream(audioStream);
    }

    delete[] audioBuffer;
}

void audioCallback(void *userdata, SDL_AudioStream *stream, int additionalAmount, int totalAmount)
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

        siner += (1.0f / (float)DEFAULT_SAMPLE_RATE) * 440.0f * SDL_powf(twelveRoot2, pitch.load());
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