#include "material.hpp"

bool Lambertian::scatter(
    const Vec3& vIn,
    const HitRecord& rec,
    Color& attenuance,
    Ray& scattered // NOLINT
) const
{
    Vec3 scatterDirection = rec.normal + randomUnitVector();

    // Catch degenerate scatter direction
    if (vec3NearZero(scatterDirection)) scatterDirection = rec.normal;

    scattered  = Ray(rec.p, scatterDirection);
    attenuance = albedo->value(rec.u, rec.v, rec.p);
    return true;
}

bool Metal::scatter(
    const Vec3& vIn,
    const HitRecord& rec,
    Color& attenuance,
    Ray& scattered) const
{
    Vec3 reflected = reflect(vIn, rec.normal);
    scattered      = Ray(rec.p, reflected + fuzz * randomInUnitSphere());
    attenuance     = albedo->value(rec.u, rec.v, rec.p);
    return (glm::dot(scattered.direction, rec.normal) > 0);
}

bool TwoSidedMaterial::scatter(
    const Vec3& vIn,
    const HitRecord& rec,
    Color& attenuance,
    Ray& scattered) const
{
    return rec.frontFace
             ? materialFront->scatter(vIn, rec, attenuance, scattered)
             : materialBack->scatter(vIn, rec, attenuance, scattered);
}

bool Dielectric::scatter(
    const Vec3& vIn,
    const HitRecord& rec,
    Color& attenuance,
    Ray& scattered) const
{
    attenuance             = Color(1.0, 1.0, 1.0);
    float refractionRatio = rec.frontFace ? (1.0 / ir) : ir;

    Vec3 unitDirection = glm::normalize(vIn);
    float cos_theta   = fmin(glm::dot(-unitDirection, rec.normal), 1.0);
    float sin_theta   = sqrt(1.0 - cos_theta * cos_theta);

    bool cannotRefract = refractionRatio * sin_theta > 1.0;
    Vec3 direction;

    if (cannotRefract
        || reflectance(cos_theta, refractionRatio) > randomFloat())
        direction = reflect(unitDirection, rec.normal);
    else
        direction = refract(unitDirection, rec.normal, refractionRatio);

    scattered = Ray(rec.p, direction);
    return true;
}
