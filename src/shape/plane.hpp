#include "hittable.hpp"
#include "material.hpp"

#include <iostream>

class Plane : public Hittable {
public:
    Plane(const Point& c, const Vec3& n, shared_ptr<Material> m);
    virtual bool hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override;

public:
    Point center;
    Vec3 normal;
    shared_ptr<Material> mat;
};
