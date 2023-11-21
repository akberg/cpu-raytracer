#pragma once

#include <glm/glm.hpp>

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <ostream>

// Types

using Vec3  = glm::vec3;
using Vec2  = glm::vec2;
using Color = glm::vec3;

[[maybe_unused]] static std::ostream&
operator<<(std::ostream& os, const Vec3& p)
{
    os << "Vec3 { " << p.x << ", " << p.y << ", " << p.z << " }";
    return os;
}

// Usings

using std::make_shared;
using std::shared_ptr;

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi       = 3.1415926535897932385f;
const float nearZero = 1e-8f;

// Utility functions

inline float degreesToRadians(float degrees) { return degrees * pi / 180.0f; }

inline int randomInt(int min, int max) { return min + rand() % (max - min); }


// `float` utilities

/// @brief Return a random float in the [0, 1)
inline float randomFloat() { return (float)rand() / ((float)RAND_MAX + 1.0f); }

inline float randomFloat(float min, float max)
{
    return min + (max - min) * randomFloat();
}

// `Vec3` utilities

inline Vec3 randomVec3()
{
    return Vec3(randomFloat(), randomFloat(), randomFloat());
}

inline Vec3 randomVec3(float min, float max)
{
    return Vec3(
        randomFloat(min, max),
        randomFloat(min, max),
        randomFloat(min, max));
}

inline Vec3 randomInUnitSphere()
{
    while (true) {
        Vec3 v = randomVec3(-1.0f, 1.0f);
        if (glm::dot(v, v) < 1.0f) return v;
    }
}

inline Vec3 randomUnitVector() { return glm::normalize(randomInUnitSphere()); }

/// @brief Return true if the vector is near zero in all dimensions
/// @param v
/// @return
inline bool vec3NearZero(const Vec3& v)
{
    const float s = nearZero;
    return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
}

inline Vec3 randomInUnitDisk()
{
    for (;;) {
        auto p = Vec3(randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f), 0.0f);
        if (glm::dot(p, p) < 1.0f) return p;
    }
}

// Vec3 ray utilities

inline Vec3 reflect(const Vec3& v, const Vec3& n)
{
    return v - 2 * glm::dot(v, n) * n;
}

/// @brief Implementing Snell's law for refraction: eta*sin(theta) =
/// etai*sin(thetai).
/// @param v
/// @param n
/// @param e eta over eta prime
/// @return
inline Vec3 refract(const Vec3& v, const Vec3& n, float e)
{
    float cosTheta = std::fmin(glm::dot(-v, n), 1.0f);
    Vec3 rOutPerp  = e * (v + cosTheta * n);
    Vec3 rOutParallel =
        -std::sqrt(std::fabs(1.0f - glm::dot(rOutPerp, rOutPerp))) * n;
    return rOutPerp + rOutParallel;
}
