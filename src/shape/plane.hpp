/// @file shape/plane.hpp
/// Implicit Shape: Infinite plane
#include "hittable.hpp"
#include "material.hpp"

#include <iostream>

class Plane : public Hittable {
public:
    Plane(const Vec3& c, const Vec3& n, shared_ptr<Material> m);
    virtual ~Plane() = default;
    virtual bool hit(const Ray& ray, double tMin, double tMax, HitRecord& rec)
        const override;
    // virtual Vec3 origo() const override { return center; }

public:
    Vec3 center;
    Vec3 normal;
    shared_ptr<Material> mat;
};
