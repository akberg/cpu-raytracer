#include "camera.hpp"
#include "image.hpp"

Camera::Camera(double aspectRatio, double focalLength)
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

Ray Camera::getRay(double u, double v) const
{
    return Ray(origin, lowerLeftCorner + u * horizontal + v * vertical - origin);
}

void Camera::render(Hittable& world, PPMImage& img, int samplesPerPixel)
{
    triIntersections = 0;
    aabbIntersections = 0;
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
    std::cerr << "Intersections: " << triIntersections + aabbIntersections
              << " (tri: " << triIntersections
              << ", aabb: " << aabbIntersections << ")";
}

void Camera::initialize()
{

}