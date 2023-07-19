#pragma once

#include <cmath>
#include <memory>
#include <limits>
#include <cstdlib>

#include <glm/vec3.hpp>

// Types

using Point = glm::dvec3;
using Vec3 = glm::dvec3;
using Color = glm::dvec3;

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
inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    return min + (max-min)*random_double();
}
