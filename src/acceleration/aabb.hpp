#pragma once

#include "rtweekend.hpp"
#include "ray.hpp"

struct Aabb {
    Point min;
    Point max;

    bool intersect(const Ray& ray, const double tMin, const double tMax) const {
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
        // std::cerr << ray << " AABB.min=" << min << " AABB.max=" << max
        //           << " tmin=" << tmin << " tmax=" << tmax
        //           //<< " r.at(tmin)=" << ray.at(tmin)
        //           << " r.at(tmax)=" << ray.at(tmax) << " tMax=" << tMax
        //           << " return "
        //           << (tmax >= tmin && tmax > 0 && tmin < tMax)
        //           << "\n";
        return tmax >= tmin && tmax > 0 && tmin < tMax; // && tmin < ray->t (?)
    }
};