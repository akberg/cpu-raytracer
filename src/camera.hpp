#pragma once

#include "ray.hpp"
#include "rtweekend.hpp"
#include "color.hpp"
#include "hittable.hpp"

#include <ostream>

class Camera {
public:
    Camera()
        : Camera(16.0 / 9.0, 1.0)
    {
    }

    Camera(double aspectRatio, double focalLength)
        : focalLength(focalLength)
        , aspectRatio(aspectRatio)
    {
        auto viewportHeight = 2.0;
        auto viewportWidth  = aspectRatio * viewportHeight;

        origin              = Point(0, 0, 0);
        horizontal          = Point(viewportWidth, 0, 0);
        vertical            = Point(0, viewportHeight, 0);
        lowerLeftCorner
            = origin - horizontal / 2.0 - vertical / 2.0 - Vec3(0, 0, focalLength);
    }

    /// @brief Get ray for the UV direction
    /// @param u Horizontal proportion, in [0, 1)
    /// @param v Vertical proportion, in [0, 1)
    /// @return Cast ray
    Ray getRay(double u, double v) const
    {
        return Ray(origin, lowerLeftCorner + u * horizontal + v * vertical - origin);
    }

    /// @brief Set aspect ratio. Updates dependent values.
    /// @param aspect Aspect ratio
    void setAspectRatio(double aspect)
    {
        this->aspectRatio = aspect;
        horizontal        = Point(aspect * vertical.y, 0, 0);
        lowerLeftCorner
            = origin - horizontal / 2.0 - vertical / 2.0 - Vec3(0, 0, focalLength);
    }

    friend std::ostream& operator<<(std::ostream& os, Camera cam)
    {
        os << "Camera(o:" << cam.origin
           << " direction: " << cam.getRay(0.5, 0.5).direction << ")";
        return os;
    }

    void render(Hittable& world, PPMImage& img, int samplesPerPixel)
    {
        for (int j = 0; j < img.height; ++j) {
            // std::cerr << "Row " << j << " / " << image_height << std::endl;
            for (int i = 0; i < img.width; ++i) {
                Color pxColor(0, 0, 0);
                for (int s = 0; s < samplesPerPixel; s++) {
                    auto u = (i + randomDouble()) / (img.width - 1);
                    auto v = (j + randomDouble()) / (img.height - 1);
                    auto r = getRay(u, v);
                    pxColor += rayColor(r, world);
                }
                img.setPixel(i, j, pxColor * (1.0 / samplesPerPixel));
            }
        }
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
