#pragma once

#include "ray.hpp"
#include "rtweekend.hpp"

#include <ostream>

class Camera {
public:
    Camera() : Camera(16.0 / 9.0, 1.0) {}
    Camera(double aspectRatio, double focalLength) : aspectRatio(aspectRatio), focalLength(focalLength) {
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

    /// @brief Get ray for the UV direction
    /// @param u Horizontal proportion, in [0, 1)
    /// @param v Vertical proportion, in [0, 1)
    /// @return Cast ray
    Ray getRay(double u, double v) const {
        return Ray(origin, lowerLeftCorner + u*horizontal + v*vertical - origin);
    }

    /// @brief Set aspect ratio. Updates dependent values.
    /// @param aspect Aspect ratio
    void setAspectRatio(double aspect) {
        this->aspectRatio = aspect;
        horizontal = Point(aspect * vertical.y, 0, 0);
        lowerLeftCorner  = origin
            - horizontal / 2.0
            - vertical / 2.0
            - Vec3(0, 0, focalLength);
    }

    friend std::ostream& operator<<(std::ostream& os, Camera cam) {
        os << "Camera(o:"<<cam.origin<<" direction: "<<cam.getRay(0.5, 0.5).direction<<")";
        return os;
    }

    /// @brief Origin of lens
    Point origin;
    /// @brief Focal Lenght affects the spreading angle of cast rays, and
    /// thereby the field of view. Lower focal length makes a wider angle, and
    /// a longer focal length makes a narrower angle.
    double focalLength;
private:
    double aspectRatio;
    Point horizontal;
    Point vertical;
    Point lowerLeftCorner;
};
