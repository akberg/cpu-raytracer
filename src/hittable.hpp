#pragma once

#include "ray.hpp"
#include "rtweekend.hpp"
#include "acceleration/aabb.hpp"

/// Forward declaration for backwards reference
class Material;

struct HitRecord {
    Vec3 p      = Vec3(0, 0, 0);
    Vec3 normal = Vec3(1, 0, 0);
    /// @brief Length of ray at intersection
    double t    = 0;
    /// @brief Texture coordinates
    double u = 0, v = 0;

    /// @brief Setting the front face when computing geometry, as opposed to
    /// computing it when colouring.
    bool frontFace;
    shared_ptr<Material> mat;

    inline void setFaceNormal(const Ray& ray, const Vec3& outwardNormal)
    {
        frontFace = glm::dot(ray.direction, outwardNormal) < 0;
        normal    = frontFace ? outwardNormal : -outwardNormal;
    }

    HitRecord() { }
    HitRecord(Vec3 p, double t, const Ray& ray, const Vec3& outwardNormal)
        : p(p)
        , t(t)
    {
        setFaceNormal(ray, outwardNormal);
    }
};

/// @brief Base class for hittable objects. Just implementing Hittable is
/// sufficient to be included in a HittableList and being used with the ray
/// tracer, but does not alone allow use in a BVH (e.g. HittableList won't need
/// that).
class Hittable {
public:
    /// @brief Compute the intersection of a ray with the primitive.
    /// @param rec HitRecord to hold further information in case of a hit.
    /// @return True if there is an intersection.
    virtual bool
    hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
};

struct Aabb;

/// @brief Base class for fully valid primitives (also hittable), that may be
/// used with a BVH.
class Primitive : public Hittable {
public:
    /// @brief Return the centre point of the primitive, used for spatial
    /// sorting.
    virtual Vec3 centroid() const           = 0;
    /// @brief Grow an AABB acording to the boundaries of the primitive.
    virtual void growAABB(Aabb& aabb) const = 0;
};
