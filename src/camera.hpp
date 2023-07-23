#pragma once

#include "rtweekend.hpp"
#include "ray.hpp"

class Camera {
public:
    Camera() : Camera(16.0 / 9.0, 1.0) {}
    Camera(double aspectRatio, double focalLength) {
        auto viewportHeight    = 2.0;
        auto viewportWidth     = aspectRatio * viewportHeight;

        origin     = Point(0, 0, 0);
        horizontal = Point(viewportWidth, 0, 0);
        vertical   = Point(0, viewportHeight, 0);
        lowerLeftCorner  = origin
            - horizontal / 2.0
            - vertical / 2.0
            - Vec3(0, 0, focalLength);
    }

    Ray getRay(double u, double v) const {
        return Ray(origin, lowerLeftCorner + u*horizontal + v*vertical - origin);
    }

private:
    Point origin;
    Point horizontal;
    Point vertical;
    Point lowerLeftCorner;
};
