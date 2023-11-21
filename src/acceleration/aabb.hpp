#pragma once

#include "ray.hpp"
#include "rtweekend.hpp"

struct HitRecord;

struct Aabb {
    Vec3 min;
    Vec3 max;

    /// @brief Hit function from Hittable (not inheriting atm)
    /// @param ray Intersecting ray
    /// @param tMin Minimum distance
    /// @param tMax Maximum distance
    /// @param rec HitRecord, for which only t is recorded yet.
    /// @return True if there is a hit.
    bool hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const;

    /// @brief Simplest intersect function. Only returns true if there is an
    /// intersection. Kept for legacy.
    /// @param ray Intersecting ray
    /// @param tMin Minimum distance
    /// @param tMax Maximum distance
    bool intersect(const Ray& ray, const double tMin, const double tMax) const;

    /// @brief Grow AABB to fit new point `pt`.
    void grow(Vec3 pt);
    /// @brief Grow AABB to fit another AABB `aabb`.
    void grow(Aabb aabb);

    /// @return Surface area of AABB
    float area() const;
};