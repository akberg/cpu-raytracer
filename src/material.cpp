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
    Vec3 refracted = refract(unitDirection, rec.normal, refractionRatio);

    scattered = Ray(rec.p, refracted);
    return true;
}
