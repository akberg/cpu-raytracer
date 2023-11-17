/// @file shape/quad.hpp
/// Implicit shape quadrilateral (technically a parallelogram).
#pragma once

#include "hittable.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "rtweekend.hpp"

class Quad : public Primitive {
public:
    Quad(Vec3 q, Vec3 u, Vec3 v, shared_ptr<Material> m)
        : q(q)
        , uEdge(u)
        , vEdge(v)
        , mat(m)
    { }
    virtual ~Quad() = default;

    bool
    hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override
    {
        quadIntersections++;
        // https://raytracing.github.io/books/RayTracingTheNextWeek.html
        // #quadrilaterals/ray-planeintersection
        auto n      = glm::cross(uEdge, vEdge);
        auto normal = glm::normalize(n);
        auto d      = glm::dot(normal, q);

        auto denom = glm::dot(normal, ray.direction);

        // Ray is parallel to the plane
        if (std::fabs(denom) < nearZero) return false;

        auto t = (d - glm::dot(normal, ray.origin)) / denom;
        // Hit point parameter is outside the ray interval
        if (t < tMin || tMax < t) return false;

        // Determine if the point lies within the planar shape using its plane
        // coordinates.
        auto isect          = ray.at(t);
        Vec3 planarHitptVec = isect - q;
        auto w              = n / glm::dot(n, n);
        auto alpha          = glm::dot(w, glm::cross(planarHitptVec, vEdge));
        auto beta           = glm::dot(w, glm::cross(uEdge, planarHitptVec));

        if (!isInterior(alpha, beta, rec)) return false;

        rec.t   = t;
        rec.p   = isect;
        rec.mat = mat;
        // TODO: Set u, v
        rec.setFaceNormal(ray, normal);
        return true;
    }

    Vec3 centroid() const override { return q + (uEdge + vEdge) / 2.0; }
    void growAABB(Aabb& aabb) const override
    {
        aabb.grow(q);
        aabb.grow(q + uEdge);
        aabb.grow(q + vEdge);
        aabb.grow(q + uEdge + vEdge);
    }

private:
    /// @brief Given the hit point in plane coordinates, return false if it is
    /// outside the primitive, otherwise set the hit record UV coordinates and
    /// return true.
    /// @param a
    /// @param b
    /// @param rec
    /// @return
    bool isInterior(double a, double b, HitRecord& rec) const
    {
        if ((a < 0) || (1 < a) || (b < 0) || (1 < b)) return false;

        rec.u = a;
        rec.v = b;
        return true;
    }
    Vec3 q;
    Vec3 uEdge, vEdge;
    shared_ptr<Material> mat;
};
