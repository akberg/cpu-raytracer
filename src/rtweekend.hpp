#pragma once

#include <glm/glm.hpp>

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <ostream>

// Types

using Point = glm::dvec3;
using Vec3  = glm::dvec3;
using Color = glm::dvec3;

[[maybe_unused]] static std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << "Vec3 { " << p.x << ", " << p.y << ", " << p.z << " }";
    return os;
}

// Usings

using std::make_shared;
using std::shared_ptr;

// Constants

const double infinity  = std::numeric_limits<double>::infinity();
const double pi        = 3.1415926535897932385;
const double near_zero = 1e-8;

// Utility functions

inline double degreesToRadians(double degrees) { return degrees * pi / 180.0; }

// `double` utilities

/// @brief Return a random float in the [0, 1)
inline double randomDouble() { return rand() / (RAND_MAX + 1.0); }

inline double randomDouble(double min, double max) { return min + (max - min) * randomDouble(); }

/// @brief Clamp a value `x` between `min` and `max`
/// @return MIN(max, MAX(min, x))
inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// `Vec3` utilities

inline Vec3 randomVec3() { return Vec3(randomDouble(), randomDouble(), randomDouble()); }

inline Vec3 randomVec3(double min, double max) {
    return Vec3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
}

inline Vec3 randomInUnitSphere() {
    while (true) {
        Vec3 v = randomVec3(-1.0, 1.0);
        if (glm::dot(v, v) < 1.0) return v;
    }
}

inline Vec3 randomUnitVector() { return glm::normalize(randomInUnitSphere()); }

/// @brief Return true if the vector is near zero in all dimensions
/// @param v
/// @return
inline bool vec3NearZero(const Vec3& v) {
    const double s = near_zero;
    return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
}

// Vec3 ray utilities

inline Vec3 reflect(const Vec3& v, const Vec3& n) { return v - 2 * glm::dot(v, n) * n; }

/// @brief Implementing Snell's law for refraction: eta*sin(theta) = etai*sin(thetai).
/// @param v
/// @param n
/// @param e eta over eta prime
/// @return
inline Vec3 refract(const Vec3& v, const Vec3& n, double e) {
    auto cosTheta     = fmin(glm::dot(-v, n), 1.0);
    Vec3 rOutPerp     = e * (v + cosTheta * n);
    Vec3 rOutParallel = -sqrt(fabs(1.0 - glm::dot(rOutPerp, rOutPerp))) * n;
    return rOutPerp + rOutParallel;
}
