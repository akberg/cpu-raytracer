#include "ray.hpp"
#include "hittable.hpp"
#include "material.hpp"
#include "rtweekend.hpp"

#include <glm/glm.hpp>

#include <ostream>
#include <sstream>
#include <string>

std::atomic_uint64_t triIntersections  = 0;
std::atomic_uint64_t aabbIntersections = 0;
shared_ptr<STBImage> rayHdri;
RayBG rayBackground   = RayBG::GRADIENT;
Color rayBgColor      = Color(1.0, 1.0, 1.0);
Vec3 rayBgLightSource = Vec3(0.0, 0.0, -1.0);

std::ostream& operator<<(std::ostream& os, const Ray& ray) {
    os << "Ray(origin: " << ray.origin << ", direction: " << ray.direction << ")";
    return os;
}

std::string logIntersections() {
    std::stringstream ss;
    uint64_t tot = aabbIntersections + triIntersections;
    ss << "Intersections : " << tot << " (AABB: " << aabbIntersections
       << ", Tri: " << triIntersections << ")";
    return ss.str();
}

Color bgRayColor(const Ray& r) {
    switch (rayBackground) {
    // case RayBG::HDRI: {
    //     if (rayHdri->height()) {
    //         Vec3 p = r.origin /
    //         auto theta = std::acos(-r.direction.y);
    //         auto phi   = std::atan2(-r.direction.z, r.direction.x) + pi;

    //         auto u = static_cast<int>(phi / (2.0 * pi) * rayHdri->width());
    //         auto v = static_cast<int>(1.0 - (theta / pi) * rayHdri->height());

    //         return rayHdri->getPixel(u, v);
    //     }
    // }
    case RayBG::SINGLE_LIGHT: {
        auto lightDir = glm::normalize(r.origin - rayBgLightSource);
        auto t        = std::clamp(glm::dot(lightDir, r.direction) + 1.0, 0.0, 1.0);
        return t * rayBgColor;
    }
    case RayBG::CONSTANT: return rayBgColor;
    case RayBG::GRADIENT:
    default: {
        auto t = 0.5 * (r.direction.y + 1.0);
        return (1.0 - t) * Color(1.0, 0.75, 0.8) + t * Color(0.2, 0.6, 1.0);
    }
    }
}

Color rayColor(const Ray& ray, const Hittable& world, int depth) {
    HitRecord rec;

    // Depth limit exceeded
    if (depth <= 0) return Color(0.0, 0.0, 0.0);

    if (world.hit(ray, 0.0001, infinity, rec)) {
        Ray scattered;
        Color attenuance;

        if (rec.mat->scatter(ray.direction, rec, attenuance, scattered))
            return attenuance * rayColor(scattered, world, depth - 1);

        return Color(0, 0, 0);
    }

    return bgRayColor(ray);
}