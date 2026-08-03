#pragma once
#include <cmath>

// Nice math implementation all in one place

namespace Math
{
    // Math constants
    constexpr float pi = 3.141592653f;
    constexpr float piOver2 = 1.570796326f; // pi/2
    constexpr float piOver4 = 0.785398163f; // pi/4

    // For comparisons
    static constexpr double epsilon = 0.000001;

    inline float pow(float x, float y)
    {
        return std::powf(x, y);
    }

    inline float sqrt(float f)
    {
        return std::sqrtf(f);
    }

    inline float floor(float f)
    {
        return std::floorf(f);
    }

    inline float ceil(float f)
    {
        return std::ceilf(f);
    }

    inline float sin(float f)
    {
        return std::sinf(f);
    }

    inline float cos(float f)
    {
        return std::cosf(f);
    }

    inline float tan(float f)
    {
        return std::tanf(f);
    }

    inline float atan2(float y, float x)
    {
        return std::atan2f(y, x);
    }

    inline float log(float f)
    {
        return std::logf(f);
    }

    inline float log10(float f)
    {
        return std::log10f(f);
    }

    inline float exp(float x)
    {
        return std::exp(x);
    }

    inline float degToRad(float d)
    {
        return d * pi / 180.0f;
    }

    inline float radToDeg(float r)
    {
        return r * 180.0f / pi;
    }

    // https://github.com/godotengine/godot/blob/master/core/math/math_funcs.h
    inline float linearToDb(float f)
    {
        return Math::log(f) * (float)8.6858896380650365530225783783321;
    }

    inline float dbToLinear(float db)
    {
        return Math::exp(db * (float)0.11512925464970228420089957273422);
        return 0;
    }

    constexpr float lerp(float a, float b, float fac)
    {
        return a + (b - a) * fac;
    }

    // Thank you Freya Holmer for the math behind this function
    // https://www.youtube.com/watch?v=LSNQuFEDOyQ
    inline float damp(float a, float b, float fac, float dt)
    {
        return (a - b) * Math::pow(fac, dt) + b;
    }

    template <class T>
    constexpr T abs(T x)
    {
        return x < 0 ? -x : x;
    }

    template <class T>
    constexpr T sign(T x)
    {
        return static_cast<T>(x == 0 ? 0 : (x < 0 ? -1 : 1));
    }

    // NOTE: Weird capitalization is to avoid (what I assume to be) a naming conflict...??
    // changing this to 'max' doesn't compile for me
    template <class T>
    constexpr T Max(T a, T b)
    {
        return static_cast<T>(a > b ? a : b);
    }

    template <class T>
    constexpr T Min(T a, T b)
    {
        return static_cast<T>(a < b ? a : b);
    }

    template<class T>
    constexpr float t_clamp(T a, T min, T max)
    {
        if (a < min)
        {
            return min;
        }
        else if (a > max)
        {
            return max;
        }
        return a;
    }

    constexpr float clampf(float a, float min, float max)
    {
        return t_clamp<float>(a, min, max);
    }
    constexpr int clampi(int a, int min, int max)
    {
        return t_clamp<int>(a, min, max);
    }

    constexpr float approach(float from, float target, float amt)
    {
        return from < target ? Math::Min(from + amt, target) : Math::Max(from - amt, target);
    }

    template <typename T, template <typename> class Vec>
    constexpr Vec<T> approach(const Vec<T> &from, const Vec<T> &target, float amt)
    {
        if (from == target)
        {
            return target;
        }

        Vec<T> vec = target - from;
        if (vec.lengthSquared() <= (amt * amt))
        {
            return target;
        }
        return from + vec.normalized() * amt;
    }

    constexpr int randRangei(int min, int max)
    {
        // Avoid dividing by zero
        int result = max - min + 1;
        if (result == 0)
            return 0;

        return (rand() % result) + min;
    }

    constexpr float randRange(float min, float max)
    {
        float denom = static_cast<float>((float)RAND_MAX / (max - min));

        // Avoid dividing by zero
        if (denom == 0.0f)
            return 0;

        return min + (static_cast<float>(rand()) / denom);
    }
}