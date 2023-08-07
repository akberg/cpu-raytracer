#pragma once

#include "rtweekend.hpp"

#include <glm/glm.hpp>

#include <ostream>

/// @brief The Ray class. Direction is normalized on creation
class Ray {
public:
    Ray() {};
    /// @brief Create a new ray
    /// @param origin
    /// @param direction direction vector, normalized on assignment
    Ray(const Point& origin, const Vec3& direction)
        : origin(origin)
        , direction(glm::normalize(direction)) {};

    friend std::ostream& operator<<(std::ostream& os, const Ray& ray);
    // {
    //     os << "Ray(origin: "<<ray.origin<<", direction: "<<ray.direction<<")";
    //     return os;
    // }

    /// @brief Ray's position at time `t`
    /// @param t
    /// @return Point vector
    Point at(double t) const { return origin + t * direction; }

public:
    /// @brief Origin of ray
    Point origin = Point(0,0,0);
    /// @brief Unit vector in ray's direction
    Vec3 direction = Vec3(1,0,0);
};

struct Hittable;

Color bgRayColor(const Ray& r);

Color rayColor(const Ray& ray, const Hittable& world, int depth = 10);
