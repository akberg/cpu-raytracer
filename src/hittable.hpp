#pragma once

#include "ray.hpp"
#include "rtweekend.hpp"

/// Forward declaration for backwards reference
class Material;

struct HitRecord {
    Point p     = Point(0, 0, 0);
    Vec3 normal = Vec3(1, 0, 0);
    double t    = 0;

    /// @brief Setting the front face when computing geometry, as opposed to
    /// computing it when colouring.
    bool frontFace;
    shared_ptr<Material> mat;

    inline void setFaceNormal(const Ray& ray, const Vec3& outwardNormal) {
        frontFace = glm::dot(ray.direction, outwardNormal) < 0;
        normal    = frontFace ? outwardNormal : -outwardNormal;
    }

    HitRecord() { }
    HitRecord(Point p, double t, const Ray& ray, const Vec3& outwardNormal)
        : p(p)
        , t(t) {
        setFaceNormal(ray, outwardNormal);
    }
};

class Hittable {
public:
    virtual bool
    hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
    virtual Point origo() const                                       = 0;
};
