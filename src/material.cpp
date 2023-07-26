#include <glm/glm.hpp>

#include "rtweekend.hpp"
#include "ray.hpp"

#include "material.hpp"

bool Lambertian::scatter(
    const Vec3& vIn, const HitRecord& rec, Color& attenuance, Ray& scattered
) const {
    Vec3 scatterDirection = rec.normal + randomUnitVector();

    // Catch degenerate scatter direction
    if (vec3NearZero(scatterDirection))
        scatterDirection = rec.normal;

    scattered = Ray(rec.p, scatterDirection);
    attenuance = albedo;
    return true;
}

bool Metal::scatter(
    const Vec3& vIn, const HitRecord& rec, Color& attenuance, Ray& scattered
) const {
    Vec3 reflected = reflect(vIn, rec.normal);
    scattered = Ray(rec.p, reflected);
    attenuance = albedo;
    return (glm::dot(scattered.direction, rec.normal) > 0);
}
