#include "sphere.hpp"

Sphere::Sphere(Point c, double r, shared_ptr<Material> m)
    : center(c)
    , radius(r)
    , mat(m) { }

bool Sphere::hit(
    const Ray& ray, double tMin, double tMax, HitRecord& rec) const {
    Vec3 oc     = ray.origin - center;
    auto a      = glm::dot(ray.direction, ray.direction);
    auto half_b = glm::dot(oc, ray.direction);
    auto c      = glm::dot(oc, oc) - radius * radius;
    auto discr  = half_b * half_b - a * c;

    if (discr < 0) {
        return false;
    }

    auto sqrtd = sqrt(discr);

    // Find the nearest riit that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < tMin || tMax < root) {
        root = (-half_b + sqrtd) / a;
        if (root < tMin || tMax < root) {
            return false;
        }
    }

    // Lots of initialisation, better be a contructor? The Rust way would be
    // returning an Option<HitRecord>
    rec.t              = root;
    rec.p              = ray.at(rec.t);
    Vec3 outwardNormal = (rec.p - center) / radius;
    rec.setFaceNormal(ray, outwardNormal);
    rec.mat = mat;
    getSphereUV(outwardNormal, rec.u, rec.v);

    return true;
}

void Sphere::getSphereUV(const Vec3& p, double& u, double& v) const {
    auto theta = std::acos(-p.y);
    auto phi   = std::atan2(-p.z, p.x) + pi;

    u = phi / (2.0 * pi);
    v = theta / pi;
}
