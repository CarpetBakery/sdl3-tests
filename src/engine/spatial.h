#pragma once
#include "math.h"

#include <string>
#include <unordered_set>

// Heavily referenced Noel Berry's Blah framework

// Forward declaration
template <class T>
class Vec2;

template <class T>
class Vec3;

template <class T>
class Rect;

template <class T>
class Line;

// Typedefs
using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;

using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;

using Rectf = Rect<float>;
using Recti = Rect<int>;

using Linef = Line<float>;
using Linei = Line<int>;

// -- Vec2 header --
template <class T>
class Vec2
{
public:
    T x, y;

    // Constructors
    constexpr Vec2();
    constexpr Vec2(T x, T y);

    template <class K>
    constexpr Vec2(const Vec2<K> &vec);

    constexpr float length() const;
    constexpr float lengthSquared() const;
    constexpr Vec2f normalized() const;
    constexpr float dot(const Vec2 &rhs) const;
    constexpr T angle() const;

    // Operator overloads
    constexpr Vec2 operator+(const Vec2 &rhs) const;
    constexpr Vec2 operator-(const Vec2 &rhs) const;
    constexpr Vec2 operator*(const T rhs) const;
    constexpr Vec2 operator*(const Vec2 &rhs) const;
    constexpr Vec2 operator/(const Vec2 &rhs) const;
    constexpr Vec2 operator/(const T rhs) const;

    // Negate
    constexpr Vec2 operator-() const;

    constexpr Vec2 &operator+=(const Vec2 &rhs);
    constexpr Vec2 &operator-=(const Vec2 &rhs);
    constexpr Vec2 &operator*=(const T rhs);
    constexpr Vec2 &operator*=(const Vec2 &rhs);
    constexpr Vec2 &operator/=(const Vec2 &rhs);
    constexpr Vec2 &operator/=(const T rhs);

    // Comparisons
    constexpr bool operator==(const Vec2 &rhs) const;
    constexpr bool operator!=(const Vec2 &rhs) const;

    std::string to_string() const;
};

// -- Vec3 header --
template <class T>
class Vec3
{
public:
    T x, y, z;

    // Constructors
    constexpr Vec3();
    constexpr Vec3(T x, T y, T z);

    template <class K>
    constexpr Vec3(const Vec3<K> &vec);

    constexpr float length() const;
    constexpr float lengthSquared() const;
    constexpr Vec3 normalized() const;
    constexpr float dot(const Vec3 &rhs) const;
    constexpr Vec3 cross(const Vec3 &rhs) const;

    // Operator overloads
    constexpr Vec3 operator+(const Vec3 &rhs) const;
    constexpr Vec3 operator-(const Vec3 &rhs) const;
    constexpr Vec3 operator*(const T rhs) const;
    constexpr Vec3 operator*(const Vec3 &rhs) const;
    constexpr Vec3 operator/(const Vec3 &rhs) const;
    constexpr Vec3 operator/(const T rhs) const;

    // Negate
    constexpr Vec3 operator-() const;

    constexpr Vec3 &operator+=(const Vec3 &rhs);
    constexpr Vec3 &operator-=(const Vec3 &rhs);
    constexpr Vec3 &operator*=(const T rhs);
    constexpr Vec3 &operator*=(const Vec3 &rhs);
    constexpr Vec3 &operator/=(const T rhs);
    constexpr Vec3 &operator/=(const Vec3 &rhs);

    // Comparisons
    constexpr bool operator==(const Vec3 &rhs) const;
    constexpr bool operator!=(const Vec3 &rhs) const;

    std::string to_string() const;
};

// -- Rect header --
template <class T>
class Rect
{
public:
    T x, y, w, h;

    // Constructors
    constexpr Rect();
    constexpr Rect(T x, T y, T w, T h);
    constexpr Rect(const Vec2<T> &pos, const Vec2<T> &size);

    template <class K>
    constexpr Rect(const Rect<K> &rect);

    constexpr Vec2<T> position() const { return Vec2<T>(x, y); }
    constexpr Vec2<T> size() const { return Vec2<T>(w, h); }

    // Easy way to get common coords
    constexpr Vec2<T> topLeft() const { return Vec2<T>(x, y); }
    constexpr Vec2<T> topMiddle() const { return Vec2<T>(x + w / 2, y); }
    constexpr Vec2<T> topRight() const { return Vec2<T>(x + w, y); }

    constexpr Vec2<T> left() const { return Vec2<T>(x, y + h / 2); }
    constexpr Vec2<T> middle() const { return Vec2<T>(x + w / 2, y + h / 2); }
    constexpr Vec2<T> right() const { return Vec2<T>(x + w, y + h / 2); }

    constexpr Vec2<T> bottomLeft() const { return Vec2<T>(x, y + h); }
    constexpr Vec2<T> bottomMiddle() const { return Vec2<T>(x + w / 2, y + h); }
    constexpr Vec2<T> bottomRight() const { return Vec2<T>(x + w, y + h); }

    // Get edges
    constexpr Line<T> leftEdge() const { return Line<T>(topLeft(), bottomLeft()); }
    constexpr Line<T> rightEdge() const { return Line<T>(topRight(), bottomRight()); }
    constexpr Line<T> bottomEdge() const { return Line<T>(bottomLeft(), bottomRight()); }
    constexpr Line<T> topEdge() const { return Line<T>(topLeft(), topRight()); }

    // Rect contains a point
    constexpr bool contains(const Vec2<T> &v) const;
    // Rect contains another rect
    constexpr bool contains(const Rect<T> &r) const;
    // Rect overlaps another rectangle
    constexpr bool overlaps(const Rect &rect) const;
    // Rect is intersected by a line
    constexpr bool intersects(const Line<T> &line) const;
    // Get the point of intersection on rect with a line, which is closest to a given point
    constexpr bool intersectsClosest(const Vec2<T> pos, const Line<T> &line, Vec2<T> *intersectionPoint = nullptr) const;

    const Rect operator+(const Vec2<T> &rhs) const;
    const Rect operator-(const Vec2<T> &rhs) const;

    // Comparisons
    constexpr bool operator==(const Rect &rhs) const;
    constexpr bool operator!=(const Rect &rhs) const;

    std::string to_string() const;
};

template <class T>
class Line
{
public:
    Vec2<T> a, b;

    constexpr Line();
    constexpr Line(const Vec2<T> &a, const Vec2<T> &b);

    template <class K>
    constexpr Line(const Line<K> &line);

    constexpr bool intersects(const Line &line, Vec2<T> *intersectionPoint = nullptr) const;
    constexpr bool intersects(const Rect<T> &rect, Vec2<T> *intersectionPoint = nullptr) const;

    // Gets the closest intersection point on a rectangle
    constexpr bool intersectsClosest(const Vec2<T> pos, const Rect<T> &rect, Vec2<T> *intersectionPoint = nullptr) const;

    constexpr Vec2f normalized() const;
    constexpr T length() const;
    constexpr T lengthSquared() const;
    constexpr T angle() const;

    const Line operator+(const Vec2<T> &rhs) const;
    const Line operator-(const Vec2<T> &rhs) const;
};

// -- Vec2 implementation --
template <class T>
constexpr Vec2<T>::Vec2()
    : x(0), y(0) {}

template <class T>
constexpr Vec2<T>::Vec2(T x, T y)
    : x(x), y(y) {}

template <class T>
template <class K>
constexpr Vec2<T>::Vec2(const Vec2<K> &vec)
    : x(static_cast<T>(vec.x)), y(static_cast<T>(vec.y)) {}

template <class T>
constexpr float Vec2<T>::length() const
{
    return Math::sqrt(lengthSquared());
}

template <class T>
constexpr float Vec2<T>::lengthSquared() const
{
    return dot(*this);
}

template <class T>
constexpr Vec2f Vec2<T>::normalized() const
{
    float len = length();
    if (len == 0)
    {
        return Vec2f(0, 0);
    }

    return Vec2f(x, y) / length();
}

template <class T>
constexpr float Vec2<T>::dot(const Vec2 &rhs) const
{
    return (x * rhs.x) + (y * rhs.y);
}

template <class T>
constexpr T Vec2<T>::angle() const
{
    return Math::atan2(y, x);
}

template <class T>
constexpr Vec2<T> Vec2<T>::operator+(const Vec2 &rhs) const
{
    return Vec2(x + rhs.x, y + rhs.y);
}

template <class T>
constexpr Vec2<T> Vec2<T>::operator-(const Vec2 &rhs) const
{
    return Vec2(x - rhs.x, y - rhs.y);
}

template <class T>
constexpr Vec2<T> Vec2<T>::operator*(const Vec2 &rhs) const
{
    return Vec2(x * rhs.x, y * rhs.y);
}

template <class T>
constexpr Vec2<T> Vec2<T>::operator*(const T rhs) const
{
    return Vec2(x * rhs, y * rhs);
}

template <class T>
constexpr Vec2<T> Vec2<T>::operator/(const Vec2 &rhs) const
{
    return Vec2(x / rhs.x, y / rhs.y);
}

template <class T>
constexpr Vec2<T> Vec2<T>::operator/(const T rhs) const
{
    return Vec2(x / rhs, y / rhs);
}

template <class T>
constexpr Vec2<T> Vec2<T>::operator-() const
{
    return Vec2(-x, -y);
}

template <class T>
constexpr Vec2<T> &Vec2<T>::operator+=(const Vec2<T> &rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

template <class T>
constexpr Vec2<T> &Vec2<T>::operator-=(const Vec2<T> &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

template <class T>
constexpr Vec2<T> &Vec2<T>::operator*=(const T rhs)
{
    x *= rhs;
    y *= rhs;
    return *this;
}

template <class T>
constexpr Vec2<T> &Vec2<T>::operator*=(const Vec2 &rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

template <class T>
constexpr Vec2<T> &Vec2<T>::operator/=(const Vec2 &rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

template <class T>
constexpr Vec2<T> &Vec2<T>::operator/=(const T rhs)
{
    x /= rhs;
    y /= rhs;
    return *this;
}

template <class T>
constexpr bool Vec2<T>::operator==(const Vec2 &rhs) const
{
    return (x == rhs.x && y == rhs.y);
}

template <class T>
constexpr bool Vec2<T>::operator!=(const Vec2 &rhs) const
{
    return !(*this == rhs);
}

template <class T>
std::string Vec2<T>::to_string() const
{
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

// -- Vec3 implementation --
template <class T>
constexpr Vec3<T>::Vec3()
    : x(0), y(0), z(0) {}

template <class T>
constexpr Vec3<T>::Vec3(T x, T y, T z)
    : x(x), y(y), z(z) {}

template <class T>
template <class K>
constexpr Vec3<T>::Vec3(const Vec3<K> &vec)
    : x(static_cast<T>(vec.x)), y(static_cast<T>(vec.y)), z(static_cast<T>(vec.z)) {}

template <class T>
constexpr float Vec3<T>::length() const
{
    return Math::sqrt(lengthSquared());
}

template <class T>
constexpr float Vec3<T>::lengthSquared() const
{
    return dot(*this);
}

template <class T>
constexpr Vec3<T> Vec3<T>::normalized() const
{
    return Vec3(x, y, z) / length();
}

template <class T>
constexpr float Vec3<T>::dot(const Vec3 &rhs) const
{
    return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
}

template <class T>
constexpr Vec3<T> Vec3<T>::cross(const Vec3 &rhs) const
{
    return Vec3(
        (y * rhs.z) - (z * rhs.y),
        (z * rhs.x) - (x * rhs.z),
        (x * rhs.y) - (y * rhs.x));
}

template <class T>
constexpr Vec3<T> Vec3<T>::operator+(const Vec3 &rhs) const
{
    return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

template <class T>
constexpr Vec3<T> Vec3<T>::operator-(const Vec3 &rhs) const
{
    return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

template <class T>
constexpr Vec3<T> Vec3<T>::operator*(const T rhs) const
{
    return Vec3(x * rhs, y * rhs, z * rhs);
}

template <class T>
constexpr Vec3<T> Vec3<T>::operator*(const Vec3 &rhs) const
{
    return Vec3(x * rhs.x, y * rhs.y, z * rhs.z);
}

template <class T>
constexpr Vec3<T> Vec3<T>::operator/(const Vec3 &rhs) const
{
    return Vec3(x / rhs.x, y / rhs.y, z / rhs.z);
}

template <class T>
constexpr Vec3<T> Vec3<T>::operator/(const T rhs) const
{
    return Vec3(x / rhs, y / rhs, z / rhs);
}

template <class T>
constexpr Vec3<T> Vec3<T>::operator-() const
{
    return Vec3(-x, -y, -z);
}

template <class T>
constexpr Vec3<T> &Vec3<T>::operator+=(const Vec3<T> &rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

template <class T>
constexpr Vec3<T> &Vec3<T>::operator-=(const Vec3<T> &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

template <class T>
constexpr Vec3<T> &Vec3<T>::operator*=(const Vec3<T> &rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

template <class T>
constexpr Vec3<T> &Vec3<T>::operator*=(const T rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

template <class T>
constexpr Vec3<T> &Vec3<T>::operator/=(const Vec3<T> &rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
}

template <class T>
constexpr Vec3<T> &Vec3<T>::operator/=(const T rhs)
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
}

template <class T>
constexpr bool Vec3<T>::operator==(const Vec3 &rhs) const
{
    // TODO: Check if we're within epsilon
    return (x == rhs.x && y == rhs.y && z == rhs.z);
}

template <class T>
constexpr bool Vec3<T>::operator!=(const Vec3 &rhs) const
{
    return !(*this == rhs);
}

template <class T>
std::string Vec3<T>::to_string() const
{
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
}

// -- Rect implementation --
template <class T>
constexpr Rect<T>::Rect()
    : x(0), y(0), w(0), h(0) {}

template <class T>
constexpr Rect<T>::Rect(T x, T y, T w, T h)
    : x(x), y(y), w(w), h(h) {}

template <class T>
constexpr Rect<T>::Rect(const Vec2<T> &pos, const Vec2<T> &size)
    : x(pos.x), y(pos.y), w(size.x), h(size.y) {}

template <class T>
template <class K>
constexpr Rect<T>::Rect(const Rect<K> &rect)
    : x(static_cast<T>(rect.x)),
        y(static_cast<T>(rect.y)),
        w(static_cast<T>(rect.w)),
        h(static_cast<T>(rect.h))
{
}

template <class T>
constexpr bool Rect<T>::contains(const Vec2<T> &v) const
{
    return (v.x >= x) && (v.x < x + w) && (v.y >= y) && (v.y < y + h);
}

template <class T>
constexpr bool Rect<T>::contains(const Rect &r) const
{
    return (r.x >= x) && (r.x + r.w < x + w) && (r.y >= y) && (r.y + r.h < y + h);
}

template <class T>
constexpr bool Rect<T>::overlaps(const Rect &r) const
{
    return x + w > r.x && y + h > r.y && x < r.x + r.w && y < r.y + r.h;
}

template <class T>
constexpr bool Rect<T>::intersects(const Line<T> &line) const
{
    return line.intersects(*this);
}

template <class T>
constexpr bool Rect<T>::intersectsClosest(const Vec2<T> pos, const Line<T> &line, Vec2<T> *intersectionPoint) const
{
    return line.intersectsClosest(pos, *this, intersectionPoint);
}

template <class T>
const Rect<T> Rect<T>::operator+(const Vec2<T> &rhs) const
{
    return Rect(x + rhs.x, y + rhs.y, w, h);
}

template <class T>
const Rect<T> Rect<T>::operator-(const Vec2<T> &rhs) const
{
    return Rect(x - rhs.x, y - rhs.y, w, h);
}

template <class T>
constexpr bool Rect<T>::operator==(const Rect &rhs) const
{
    return (
        x == rhs.x &&
        y == rhs.y &&
        w == rhs.w &&
        h == rhs.h);
}

template <class T>
constexpr bool Rect<T>::operator!=(const Rect &rhs) const
{
    return !(*this == rhs);
}

template <class T>
std::string Rect<T>::to_string() const
{
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(w) + ", " + std::to_string(h) + ")";
}

// -- Line implementation --
template <class T>
constexpr Line<T>::Line()
    : a(Vec2<T>(0, 0)), b(Vec2<T>(0, 0)) {}

template <class T>
constexpr Line<T>::Line(const Vec2<T> &a, const Vec2<T> &b)
    : a(a), b(b) {}

template <class T>
template <class K>
constexpr Line<T>::Line(const Line<K> &line)
    : a(static_cast<Vec2<T>>(a)), b(static_cast<Vec2<T>>(b)) {}

// https://www.geeksforgeeks.org/cpp/line-intersection-in-cpp/
template <class T>
constexpr bool Line<T>::intersects(const Line &line, Vec2<T> *intersectionPoint) const
{
    // Checks if a point lies on a given line segment
    auto onSegment = [&](const Vec2<T> &p, const Vec2<T> &q, const Vec2<T> &r) -> bool
    {
        if (q.x <= Math::Max(p.x, r.x) &&
            q.x >= Math::Min(p.x, r.x) &&
            q.y <= Math::Max(p.y, r.y) &&
            q.y >= Math::Min(p.y, r.y))
        {
            return true;
        }
        return false;
    };

    // Find the orientation of the ordered triplet
    auto orientation = [&](const Vec2<T> &p, const Vec2<T> &q, const Vec2<T> &r) -> int
    {
        double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
        if (val == 0)
        {
            // Collinear
            return 0;
        }

        // Clock or counterclock wise
        return (val > 0) ? 1 : 2;
    };

    Vec2<T> res;

    // Find the four orientations needed for general and
    // special cases
    int o1 = orientation(a, b, line.a);
    int o2 = orientation(a, b, line.b);
    int o3 = orientation(line.a, line.b, a);
    int o4 = orientation(line.a, line.b, b);

    // General case: lines intersect if they have different
    // orientations
    if (o1 != o2 && o3 != o4)
    {
        // Compute intersection point
        double a1 = b.y - a.y;
        double b1 = a.x - b.x;
        double c1 = a1 * a.x + b1 * a.y;

        double a2 = line.b.y - line.a.y;
        double b2 = line.a.x - line.b.x;
        double c2 = a2 * line.a.x + b2 * line.a.y;

        double determinant = a1 * b2 - a2 * b1;

        if (determinant != 0)
        {
            res.x = (c1 * b2 - c2 * b1) / determinant;
            res.y = (a1 * c2 - a2 * c1) / determinant;

            if (intersectionPoint)
            {
                *intersectionPoint = res;
            }
            return true;
        }
    }

    // Special Cases: check if the lines are collinear and overlap
    if (o1 == 0 && onSegment(a, line.a, b))
        return true;
    if (o2 == 0 && onSegment(a, line.b, b))
        return true;
    if (o3 == 0 && onSegment(line.a, a, line.b))
        return true;
    if (o4 == 0 && onSegment(line.a, b, line.b))
        return true;

    // Lines do not intersect in any case
    return false;
}

template <class T>
constexpr bool Line<T>::intersects(const Rect<T> &rect, Vec2<T> *intersectionPoint) const
{
    return (
        intersects(rect.leftEdge(), intersectionPoint) ||
        intersects(rect.rightEdge(), intersectionPoint) ||
        intersects(rect.topEdge(), intersectionPoint) ||
        intersects(rect.bottomEdge(), intersectionPoint));
}

template <class T>
constexpr bool Line<T>::intersectsClosest(const Vec2<T> pos, const Rect<T> &rect, Vec2<T> *intersectionPoint) const
{
    Vec2<T> checkPoint;
    Vec2<T> closestPoint;
    float closestLength = -1;

    bool collided = false;

    constexpr uint8_t lineCount = 4;
    Line<T> lines[lineCount] = {
        rect.leftEdge(),
        rect.rightEdge(),
        rect.topEdge(),
        rect.bottomEdge()};

    // Check all lines in rect to see if any are colliding with this line
    for (int i = 0; i < lineCount; i++)
    {
        if (intersects(lines[i], &checkPoint))
        {
            collided = true;

            float checkLength = (pos - checkPoint).lengthSquared();
            if (closestLength < 0 || checkLength < closestLength)
            {
                closestPoint = checkPoint;
                closestLength = checkLength;
            }
        }
    }

    *intersectionPoint = closestPoint;
    return collided;
}

template <class T>
constexpr Vec2f Line<T>::normalized() const
{
    return (b - a).normalized();
}

template <class T>
constexpr T Line<T>::length() const
{
    return (b - a).length();
}

template <class T>
constexpr T Line<T>::lengthSquared() const
{
    return (b - a).lengthSquared();
}

template <class T>
constexpr T Line<T>::angle() const
{
    return (b - a).angle() * -1;
}

template <class T>
const Line<T> Line<T>::operator+(const Vec2<T> &rhs) const
{
    return Line(a + rhs, b + rhs);
}

template <class T>
const Line<T> Line<T>::operator-(const Vec2<T> &rhs) const
{
    return Line(a - rhs, b - rhs);
}

// Hash defs
// https://stackoverflow.com/questions/59060054/implement-hash-for-custom-class-c
template <class T>
struct std::hash<Vec2<T>>
{
	size_t operator()(Vec2<T> const &vec) const noexcept
	{
		std::hash<T> hasher{};
		size_t h1 = hasher(vec.x);
		size_t h2 = hasher(vec.y);

		// Combine with XOR
		return h1 ^ (h2 << 1);
	}
};
