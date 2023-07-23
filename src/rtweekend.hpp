#pragma once

#include <cmath>
#include <memory>
#include <limits>
#include <cstdlib>
#include <ostream>

#include <glm/vec3.hpp>

// Types

using Point = glm::dvec3;
using Vec3 = glm::dvec3;
using Color = glm::dvec3;

std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << "Vec3 { " << p.x << ", " << p.y << ", " << p.z << " }";
    return os;
}

// Usings

using std::shared_ptr;
using std::make_shared;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility functions

inline double degreesToRadians(double degrees) {
    return degrees * pi / 180.0;
}

/**Return a random float in the [0, 1) */
inline double randomDouble() {
    return rand() / (RAND_MAX + 1.0);
}

inline double randomDouble(double min, double max) {
    return min + (max-min)*randomDouble();
}

inline Vec3 randomVec3() {
    return Vec3(randomDouble(), randomDouble(), randomDouble());
}

inline Vec3 randomVec3(double min, double max) {
    return Vec3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
}

inline Vec3 randomInUnitSphere() {
    while (true) {
        Vec3 v = randomVec3(-1.0, 1.0);
        if (glm::length(v)*glm::length(v) < 1.0)
            return v;
    }
}
// inline Vec3 randomInUnitSphere() {
//     return glm::normalize(randomVec3(0.0, 1.0));
// }

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
