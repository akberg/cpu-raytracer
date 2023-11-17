/// @file shape/sphere.hpp
/// Implicit Shape: Sphere
#pragma once

#include "hittable.hpp"
#include "rtweekend.hpp"

#include <glm/glm.hpp>

class Sphere : public Hittable {
public:
    Sphere() { }
    Sphere(Vec3 c, double r, shared_ptr<Material> m)
        : center(c)
        , radius(r)
        , mat(m)
    { }
    virtual ~Sphere() = default;

    void getSphereUV(const Vec3& p, double& u, double& v) const;

    virtual bool hit(const Ray& ray, double tMin, double tMax, HitRecord& rec)
        const override;
    // virtual Vec3 origo() const override { return center; }

public:
    Vec3 center;
    double radius;
    Vec3 rotation;
    shared_ptr<Material> mat;
};
