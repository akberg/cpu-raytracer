#include "aabb.hpp"
#include "hittable.hpp"
#include "ray.hpp"
#include "rtweekend.hpp"

bool
Aabb::hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const {
    aabbIntersections++; // Counting total no. of intersections.
    double tx1  = (min.x - ray.origin.x) / ray.direction.x;
    double tx2  = (max.x - ray.origin.x) / ray.direction.x;
    double tmin = std::min(tx1, tx2);
    double tmax = std::max(tx1, tx2);
    double ty1  = (min.y - ray.origin.y) / ray.direction.y;
    double ty2  = (max.y - ray.origin.y) / ray.direction.y;
    tmin        = std::max(tmin, std::min(ty1, ty2));
    tmax        = std::min(tmax, std::max(ty1, ty2));
    double tz1  = (min.z - ray.origin.z) / ray.direction.z;
    double tz2  = (max.z - ray.origin.z) / ray.direction.z;
    tmin        = std::max(tmin, std::min(tz1, tz2));
    tmax        = std::min(tmax, std::max(tz1, tz2));

    bool hit = tmax >= tmin && tmax > 0 && tmin < tMax;
    rec.t = hit ? tmin : 1e30;
    return hit;
}

bool Aabb::intersect(
    const Ray& ray,
    const double tMin,
    const double tMax) const {

    HitRecord rec;
    return hit(ray, tMin, tMax, rec);
}

void Aabb::grow(Vec3 pt) {
    min = glm::min(min, pt);
    max = glm::max(max, pt);
}

float Aabb::area() const {
    Vec3 e = max - min;
    return e.x * e.y + e.y * e.z + e.z * e.x;
}
