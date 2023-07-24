#pragma once

#include "material.hpp"

#include "ray.hpp"
#include "rtweekend.hpp"

class Material;

struct HitRecord {
    Point p;
    Vec3 normal;
    double t;
    /**Setting the front face when computing geometry, as opposed to computing
     * it when colouring.
    */
    bool frontFace;
    shared_ptr<Material> mat;

    inline void setFaceNormal(const Ray& ray, const Vec3& outwardNormal) {
        frontFace = glm::dot(ray.direction, outwardNormal) < 0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }

    HitRecord() {}
    HitRecord(Point p, double t, const Ray& ray, const Vec3& outwardNormal)
    : p(p), t(t) {
        setFaceNormal(ray, outwardNormal);
    }
};

class Hittable {
public:
    virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
};
