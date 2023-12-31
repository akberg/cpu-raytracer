/// @file shape/triangle.hpp
/// Currently defines both Triangle as an independent primitive, and the
/// combination Mesh and IndexedTriangle, an attempt at figuring out how complex
/// meshes are better stored for ray tracing.
#pragma once

#include "hittable.hpp"
#include "rtweekend.hpp"

#include <glm/glm.hpp>
#include <vector>

/// @brief Triangle as implicit shape, containing all needed attributes to work
/// on its own. Current size: 80 bytes (44 if using f32)
class Triangle : public Primitive {
public:
    Triangle(Vec3 v0, Vec3 v1, Vec3 v2, shared_ptr<Material> m)
        : mCentroid((v0 + v1 + v2) * 0.333f)
        , mat(m) {
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
    }
    virtual ~Triangle() = default;

    /// @brief Mõller-Trumbore triangle intersection. Assumes clockwise normal.
    /// @return
    bool
    hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const override {
        triIntersections++; // Counting total no. of intersections.
        const float epsilon = nearZero;

        const auto edge1 = vertices[1] - vertices[0];
        const auto edge2 = vertices[2] - vertices[0];
        const auto h     = glm::cross(r.direction, edge2);
        const auto a     = glm::dot(edge1, h);

        if (-epsilon < a && a < epsilon) return false; // Ray parallel

        const auto f = 1 / a;
        const auto s = r.origin - vertices[0];
        const auto u = f * glm::dot(s, h);

        if (u < 0 || 1 < u) return false;

        const auto q = glm::cross(s, edge1);
        const auto v = f * glm::dot(r.direction, q);

        if (v < 0 || 1 < u + v) return false;

        const auto t = f * glm::dot(edge2, q);

        if (t < tMin || tMax < t) return false;

        rec.t       = t;
        rec.p       = r.at(t);
        Vec3 normal = glm::cross(edge1, edge2);
        rec.setFaceNormal(r, normal);
        rec.mat = mat;
        return true;
    }
    Vec3 centroid() const override { return mCentroid; }
    void growAABB(Aabb& aabb) const override {
        aabb.grow(vertices[0]);
        aabb.grow(vertices[1]);
        aabb.grow(vertices[2]);
    }
public:
    Vec3 vertices[3];
    Vec3 mCentroid;
    shared_ptr<Material> mat;
};
