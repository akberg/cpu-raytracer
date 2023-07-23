#pragma once

#include <glm/glm.hpp>

#include "rtweekend.hpp"

/// @brief The Ray class
class Ray {
public:
    Ray() {};
    /// @brief Create a new ray
    /// @param origin
    /// @param direction direction vector, normalized on assignment
    Ray(const Point& origin, const Vec3& direction)
        : origin(origin)
        , direction(glm::normalize(direction)) {};

    /// @brief Ray's position at time `t`
    /// @param t
    /// @return Point vector
    Point at(double t) const { return origin + t * direction; }

public:
    Point origin;
    Vec3 direction;
};
