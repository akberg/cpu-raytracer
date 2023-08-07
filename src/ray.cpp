#include "ray.hpp"
#include "hittable.hpp"
#include "material.hpp"
#include "rtweekend.hpp"

#include <glm/glm.hpp>

#include <ostream>

std::ostream& operator<<(std::ostream& os, const Ray& ray)
{
    os << "Ray(origin: " << ray.origin << ", direction: " << ray.direction << ")";
    return os;
}

Color bgRayColor(const Ray& r)
{
    Vec3 unit_direction = glm::normalize(r.direction);
    auto t              = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - t) * Color(1.0, 0.75, 0.8) + t * Color(0.2, 0.6, 1.0);
}

Color rayColor(const Ray& ray, const Hittable& world, int depth)
{
    HitRecord rec;

    // Depth limit exceeded
    if (depth <= 0) return Color(0.0, 0.0, 0.0);

    if (world.hit(ray, 0.0001, infinity, rec)) {
        Ray scattered;
        Color attenuance;

        if (rec.mat->scatter(ray.direction, rec, attenuance, scattered))
            return attenuance * rayColor(scattered, world, depth - 1);

        return Color(0, 0, 0);
    }

    return bgRayColor(ray);
}