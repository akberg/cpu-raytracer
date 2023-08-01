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
    scattered = Ray(rec.p, reflected + fuzz*randomInUnitSphere());
    attenuance = albedo;
    return (glm::dot(scattered.direction, rec.normal) > 0);
}

bool TwoSidedMaterial::scatter(
    const Vec3& vIn, const HitRecord& rec, Color& attenuance, Ray& scattered
) const {
    if (rec.frontFace)
        return materialFront->scatter(vIn, rec, attenuance, scattered);
    else
        return materialBack->scatter(vIn, rec, attenuance, scattered);
}

bool Dielectric::scatter(
    const Vec3& vIn, const HitRecord& rec, Color& attenuance, Ray& scattered
) const {
    attenuance = Color(1.0, 1.0, 1.0);
    double refractionRatio = rec.frontFace ? (1.0/ir) : ir;

    Vec3 unitDirection = glm::normalize(vIn);
    double cos_theta = fmin(glm::dot(-unitDirection, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

    bool cannotRefract = refractionRatio * sin_theta > 1.0;
    Vec3 direction;

    if (cannotRefract || reflectance(cos_theta, refractionRatio) > randomDouble())
        direction = reflect(unitDirection, rec.normal);
    else
        direction = refract(unitDirection, rec.normal, refractionRatio);

    scattered = Ray(rec.p, direction);
    return true;
}
