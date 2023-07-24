#pragma once

#include "ray.hpp"
#include "rtweekend.hpp"


// If we want different objects to have different materials, we have a design
// decision. We could have a universal material with lots of parameters and
// different material types just zero out some of those parameters.
// -> would make it easier to define materials in configs or runtime
// Or we could have an abstract material class that encapsulates behavior.
// -> might give cleaner code

class Material {
public:
    virtual bool scatter(
        const Vec3& vIn, const Vec3& normal, const Point& p, Color& attenuance, Ray& scattered
    ) const = 0;
};

class Lambertian : public Material {
public:
    Lambertian(const Color& a) : albedo(a) {}

    /// @brief Calculate scatter ray according to material properties
    /// @param vIn Incoming ray
    /// @param normal Normal at hit point
    /// @param p Hit point
    /// @param attenuance Color of material
    /// @param scattered Return scattered ray
    /// @return true if valid
    bool scatter(
        const Vec3& vIn, const Vec3& normal, const Point& p, Color& attenuance, Ray& scattered
    ) const override;

private:
    Color albedo;
};

class Metal : public Material {
public:
    Metal(const Color& a) : albedo(a) {}

    /// @brief Calculate scatter ray according to material properties
    /// @param vIn Incoming ray
    /// @param normal Normal at hit point
    /// @param p Hit point
    /// @param attenuance Color of material
    /// @param scattered Return scattered ray
    /// @return true if valid
    bool scatter(
        const Vec3& vIn, const Vec3& normal, const Point& p, Color& attenuance, Ray& scattered
    ) const override;

private:
    Color albedo;
};
