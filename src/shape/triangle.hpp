#pragma once

#include "hittable.hpp"
#include "rtweekend.hpp"

#include <glm/glm.hpp>

class Triangle : public Hittable {
public:
    Triangle(Point v0, Point v1, Point v2, shared_ptr<Material> m) : mat(m) {
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
        centroid = (vertices[0] + vertices[1] + vertices[2]) * 0.3333;
    }
    virtual ~Triangle() = default;
    /// @brief Mõller-Trumbore riangle intersection. Assumes clockwise normal.
    /// @return
    virtual bool
    hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
        const double epsilon = 0.0001;

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
    virtual Point origo() const override { return centroid; }

private:
    Point vertices[3];
    Point centroid;
    shared_ptr<Material> mat;
};