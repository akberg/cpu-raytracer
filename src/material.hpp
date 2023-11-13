#pragma once

#include "hittable.hpp" // HitRecord
#include "ray.hpp"      // Ray
#include "rtweekend.hpp"
#include "texture.hpp"

// TODO: Add texture coordinates in some way

// If we want different objects to have different materials, we have a design
// decision. We could have a universal material with lots of parameters and
// different material types just zero out some of those parameters.
// -> would make it easier to define materials in configs or runtime
// Or we could have an abstract material class that encapsulates behavior.
// -> might give cleaner code

class Material {
public:
    virtual bool scatter(
        const Vec3& vIn,
        const HitRecord& rec,
        Color& attenuance,
        Ray& scattered) const = 0;
    virtual ~Material()       = default;
    virtual std::string name() const { return "Unnamed Material"; };
};

// ----------------------------------------------------------------------------/

class Lambertian : public Material {
public:
    Lambertian(const Color& a) : albedo(make_shared<SolidColorTexture>(a)) { }
    Lambertian(shared_ptr<Texture> a) : albedo(a) { }
    std::string name() const override { return "Lambertian"; }

    /// @brief Calculate scatter ray according to material properties
    /// @param vIn Incoming ray
    /// @param normal Normal at hit point
    /// @param p Hit point
    /// @param attenuance Color of material
    /// @param scattered Return scattered ray
    /// @return true if valid
    bool scatter(
        const Vec3& vIn,
        const HitRecord& rec,
        Color& attenuance,
        Ray& scattered) const override;
public:
    shared_ptr<Texture> albedo;
};

class Metal : public Material {
public:
    Metal(shared_ptr<Texture> a, double f) : albedo(a), fuzz(f < 1 ? f : 1) { }
    Metal(shared_ptr<Texture> a) : Metal(a, 0.0) { }
    Metal(const Color& a, double f)
        : Metal(make_shared<SolidColorTexture>(a), f) { }
    Metal(const Color& a) : Metal(make_shared<SolidColorTexture>(a)) { }
    std::string name() const override { return "Metal"; }

    /// @brief Calculate scatter ray according to material properties
    /// @param vIn Incoming ray
    /// @param normal Normal at hit point
    /// @param p Hit point
    /// @param attenuance Color of material
    /// @param scattered Return scattered ray
    /// @return true if valid
    bool scatter(
        const Vec3& vIn,
        const HitRecord& rec,
        Color& attenuance,
        Ray& scattered) const override;

    double getFuzz() const { return fuzz; }
    double setFuzz(double f) {
        fuzz = f < 1 ? f : 1;
        return fuzz;
    }

public:
    shared_ptr<Texture> albedo;

private:
    double fuzz = 0;
};

class TwoSidedMaterial : public Material {
public:
    TwoSidedMaterial(shared_ptr<Material> mFront, shared_ptr<Material> mBack)
        : materialFront(mFront)
        , materialBack(mBack) { }
    std::string name() const override { return "TwoSided"; }

    /// @brief Calculate scatter ray according to material properties
    /// @param vIn Incoming ray
    /// @param normal Normal at hit point
    /// @param p Hit point
    /// @param attenuance Color of material
    /// @param scattered Return scattered ray
    /// @return true if valid
    bool scatter(
        const Vec3& vIn,
        const HitRecord& rec,
        Color& attenuance,
        Ray& scattered) const override;

public:
    shared_ptr<Material> materialFront;
    shared_ptr<Material> materialBack;
};

class Dielectric : public Material {
public:
    Dielectric(double refractionIndex) : ir(refractionIndex) { }
    std::string name() const override { return "Dielectric"; }

    virtual bool scatter(
        const Vec3& vIn,
        const HitRecord& rec,
        Color& attenuance,
        Ray& scattered) const override;

public:
    /// @brief Index of refraction
    double ir;

private:
    static double reflectance(double cosine, double refIdx) {
        // Schlick's approximation for reflectance
        auto r0 = (1 - refIdx) / (1 + refIdx);
        r0      = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5.0);
    }
};
