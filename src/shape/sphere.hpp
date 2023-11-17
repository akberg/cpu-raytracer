/// @file shape/sphere.hpp
/// Implicit Shape: Sphere
#pragma once

#include "hittable.hpp"
#include "rtweekend.hpp"

#include <glm/glm.hpp>

class Sphere : public Primitive {
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
    virtual Vec3 centroid() const override { return center; }
    virtual void growAABB(Aabb& aabb) const override {
        aabb.grow(center + Vec3(radius));
        aabb.grow(center - Vec3(radius));
    }

public:
    Vec3 center;
    double radius;
    Vec3 rotation;
    shared_ptr<Material> mat;
};
