/// @file shape/sphere.hpp
/// Implicit Shape: Sphere
#pragma once

#include "hittable.hpp"
#include "rtweekend.hpp"

#include <glm/glm.hpp>

class Sphere : public Hittable {
public:
    Sphere() { }
    Sphere(Point c, double r, shared_ptr<Material> m);
    virtual ~Sphere() = default;

    void getSphereUV(const Vec3& p, double& u, double& v) const;

    virtual bool hit(
        const Ray& ray, double tMin, double tMax, HitRecord& rec) const override;
    // virtual Point origo() const override { return center; }

public:
    Point center;
    double radius;
    shared_ptr<Material> mat;
};
