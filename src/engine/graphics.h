#pragma once
#include "spatial.h"

#include <string>


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