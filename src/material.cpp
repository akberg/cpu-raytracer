#include <glm/glm.hpp>

#include "rtweekend.hpp"
#include "ray.hpp"

#include "material.hpp"

bool Lambertian::scatter(
    const Vec3& vIn, const Vec3& normal, const Point& p, Color& attenuance, Ray& scattered
) const {
    Vec3 scatterDirection = normal + randomUnitVector();

    // Catch degenerate scatter direction
    if (vec3NearZero(scatterDirection))
        scatterDirection = normal;

    scattered = Ray(p, scatterDirection);
    attenuance = albedo;
    return true;
}

bool Metal::scatter(
    const Vec3& vIn, const Vec3& normal, const Point& p, Color& attenuance, Ray& scattered
) const {
    Vec3 reflected = reflect(vIn, normal);
    scattered = Ray(p, reflected);
    attenuance = albedo;
    return (glm::dot(scattered.direction, normal) > 0);
}
