#include "camera.hpp"
#include "color.hpp"
#include "hittableList.hpp"
#include "ray.hpp"
#include "rtweekend.hpp"
#include "shape/sphere.hpp"

#include <glm/glm.hpp>

#include <iostream>

#define MAX_RAY_DEPTH 8


Color bgRayColor(const Ray& r)
{
    Vec3 unit_direction = glm::normalize(r.direction);
    auto t              = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - t) * Color(1.0, 0.75, 0.8) + t * Color(0.2, 0.6, 1.0);
}

Color rayColor(const Ray& ray, const Hittable& world, int depth = MAX_RAY_DEPTH)
{
    HitRecord rec;

    // Depth limit exceeded
    if (depth <= 0)
        return Color(0.0, 0.0, 0.0);

    if (world.hit(ray, 0.0001, infinity, rec)) {
        Ray scattered;
        Color attenuance;

        if (rec.mat->scatter(ray.direction, rec.normal, rec.p, attenuance, scattered))
            return attenuance * rayColor(scattered, world, depth - 1);

        return Color(0,0,0);
    }
    // if (depth < 40)
    //     std::cerr << "d " << depth << " no hit, add bg " << bgRayColor(ray) << std::endl;

    return bgRayColor(ray);
}

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        std::cerr << "Unexpected argument: " << argv[i] << std::endl;
    }

    // Image

    const auto aspect_ratio   = 16.0 / 9.0;
    const int image_width     = 400;
    const int image_height    = static_cast<int>(image_width / aspect_ratio);
    const int samplesPerPixel = 100;

    // World
    auto matLambRed = make_shared<Lambertian>(Color(1.0, 0.05, 0.05));
    auto matMetRed = make_shared<Metal>(Color(1.0, 0.6, 0.6));
    auto matLambGreen = make_shared<Lambertian>(Color(0.05, 1.0,0.05));
    HittableList world;
    world.add(make_shared<Sphere>(Point(1, 0, -1), 0.5, matMetRed));
    world.add(make_shared<Sphere>(Point(-1, 0, -1), 0.5, matLambRed));
    world.add(make_shared<Sphere>(Point(0, -100.5, -1), 100.0, matLambGreen));

    // Camera

    auto viewport_height = 2.0;
    auto viewport_width  = aspect_ratio * viewport_height;
    auto focal_length    = 1.0;

    Camera cam;

    // Render

    std::cout << "P3\n"
              << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "Row " << image_height - j << " / " << image_height << std::endl;
        for (int i = 0; i < image_width; ++i) {
            Color pxColor(0, 0, 0);
            for (int s = 0; s < samplesPerPixel; s++) {
                auto u = (i + randomDouble()) / (image_width - 1);
                auto v = (j + randomDouble()) / (image_height - 1);
                auto r = cam.getRay(u, v);
                pxColor += rayColor(r, world);
            }
            // Color px_color(double(i)/(image_width-1), double(j)/(image_height-1), 0.25);
            writeColor(std::cout, pxColor, samplesPerPixel);
        }
    }
}