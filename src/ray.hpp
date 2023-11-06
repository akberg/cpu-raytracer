#pragma once

#include "rtweekend.hpp"
#include "image.hpp"

#include <glm/glm.hpp>

#include <atomic>
#include <ostream>

/// @brief The Ray class. Direction is normalized on creation
class Ray {
public:
    Ray() {};

    /// @brief Create a new ray
    /// @param origin
    /// @param direction direction vector, normalized on assignment
    Ray(const Point& origin, const Vec3& direction, double tMin, double tMax)
        : origin(origin)
        , direction(glm::normalize(direction))
        , tMin(tMin)
        , tMax(tMax) {};

    Ray(const Point& origin, const Vec3& direction)
        : Ray(origin, direction, -infinity, infinity) {};

    friend std::ostream& operator<<(std::ostream& os, const Ray& ray);

    /// @brief Ray's position at time `t`
    /// @param t
    /// @return Point vector
    Point at(double t) const { return origin + t * direction; }

public:
    /// @brief Origin of ray
    Point origin = Point(0, 0, 0);
    /// @brief Unit vector in ray's direction
    Vec3 direction = Vec3(1, 0, 0);
    double tMin = -infinity;
    double tMax = infinity;
};

enum class RayBG { GRADIENT, HDRI };
extern RayBG rayBackground;
extern shared_ptr<STBImage> rayHdri;

extern std::atomic_uint64_t triIntersections;
extern std::atomic_uint64_t aabbIntersections;

struct Hittable;

Color bgRayColor(const Ray& r);

Color rayColor(const Ray& ray, const Hittable& world, int depth = 10);
