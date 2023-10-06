#include "plane.hpp"
#include "ray.hpp"
#include "rtweekend.hpp"

#include <glm/glm.hpp>

Plane::Plane(const Point& c, const Vec3& n, shared_ptr<Material> m)
    : center(c)
    , normal(n)
    , mat(m) { }

bool Plane::hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const {
    auto denom = glm::dot(normal, ray.direction); // < 0

    if (fabs(denom) < nearZero) return false;

    auto p0l0 = center - ray.origin;
    auto t    = glm::dot(p0l0, normal) / denom;

    if (t < tMin || tMax < t) return false;

    rec.t = t;
    rec.p = ray.at(rec.t);
    rec.setFaceNormal(ray, normal);
    rec.mat = mat;

    return true;
}
