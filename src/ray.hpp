#pragma once

#include "image.hpp"
#include "rtweekend.hpp"

#include <glm/glm.hpp>

#include <atomic>
#include <ostream>
#include <string>

/// @brief The Ray class. Direction is normalized on creation
class Ray {
public:
    Ray() {};

    /// @brief Create a new ray
    /// @param origin
    /// @param direction direction vector, normalized on assignment
    Ray(const Vec3& origin, const Vec3& direction, double tMin, double tMax)
        : origin(origin)
        , direction(glm::normalize(direction))
        , tMin(tMin)
        , tMax(tMax) {};

    Ray(const Vec3& origin, const Vec3& direction) : Ray(origin, direction, -infinity, infinity) {};

    friend std::ostream& operator<<(std::ostream& os, const Ray& ray);

    /// @brief Ray's position at time `t`
    /// @param t
    /// @return Vec3 vector
    Vec3 at(double t) const { return origin + t * direction; }

public:
    /// @brief Origin of ray
    Vec3 origin    = Vec3(0, 0, 0);
    /// @brief Unit vector in ray's direction
    Vec3 direction = Vec3(1, 0, 0);
    double tMin    = -infinity;
    double tMax    = infinity;
};

enum class RayBG { GRADIENT, HDRI, CONSTANT, SINGLE_LIGHT };
extern RayBG rayBackground;
extern shared_ptr<STBImage> rayHdri;
extern Color rayBgColor;
extern Vec3 rayBgLightSource;

extern std::atomic_uint64_t triIntersections;
extern std::atomic_uint64_t aabbIntersections;

std::string logIntersections();

class Hittable;

Color bgRayColor(const Ray& r);

Color rayColor(const Ray& ray, const Hittable& world, int depth = 10);
