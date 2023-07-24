#pragma once

#include "hittable.hpp"
#include "rtweekend.hpp"

#include <glm/glm.hpp>


class Sphere : public Hittable {
public:
    Sphere() {}
    Sphere(Point c, double r, shared_ptr<Material> m) : center(c), radius(r), mat(m) {}

    virtual bool hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override;

public:
    Point center;
    double radius;
    shared_ptr<Material> mat;
};
