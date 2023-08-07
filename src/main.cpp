#include "camera.hpp"
#include "color.hpp"
#include "hittableList.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "rtweekend.hpp"
#include "shape/plane.hpp"
#include "shape/sphere.hpp"

#include <glm/glm.hpp>

#include <iostream>


int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        std::cerr << "Unexpected argument: " << argv[i] << std::endl;
    }

    // Image

    const auto aspect_ratio   = 16.0 / 9.0;
    const int image_width     = 1920;
    const int image_height    = static_cast<int>(image_width / aspect_ratio);
    const int samplesPerPixel = 70;

    // World
    auto matLambDark = make_shared<Lambertian>(Color(0.1, 0.1, 0.1));
    auto matLambRed = make_shared<Lambertian>(Color(1.0, 0.05, 0.05));
    auto matLambGreen = make_shared<Lambertian>(Color(0.05, 1.0,0.05));
    auto matLambBlue = make_shared<Lambertian>(Color(0.05, 0.05,1.0));
    auto matMetRed = make_shared<Metal>(Color(1.0, 0.6, 0.6));
    auto matMetRed2 = make_shared<Metal>(Color(1.0, 0.6, 0.6), 0.5);
    auto matMetDark = make_shared<Metal>(Color(0.5, 0.5, 0.5), 0.2);
    auto matRG     = make_shared<TwoSidedMaterial>(matMetDark, matLambGreen);
    auto dielectric = make_shared<Dielectric>(1.5);
    HittableList world;
    // Visualise axes:
    // world.add(make_shared<Sphere>(Point( 0.5, -0.5, -2), 0.2, matLambRed));
    // world.add(make_shared<Sphere>(Point(-0.5,  0.5, -2), 0.2, matLambGreen));
    // world.add(make_shared<Sphere>(Point(-0.5, -0.5, -1), 0.2, matLambBlue));
    // world.add(make_shared<Sphere>(Point(-0.5, -0.5, -2), 0.2, matLambDark));

    world.add(make_shared<Sphere>(Point(-1, 0, -1), -0.5, dielectric));
    world.add(make_shared<Sphere>(Point(1, 0, -1), 0.5, matMetRed));
    // world.add(make_shared<Sphere>(Point(0, 0, -1), 0.5, matLambRed));
    world.add(make_shared<Sphere>(Point(0, 0, -1), 0.5, matMetRed2));
    world.add(make_shared<Sphere>(Point(0, -50.5, -1), 50.0, matMetDark));
    world.add(make_shared<Plane>(Point(0, -0.5, -4), Vec3(0, 1, 0), matRG));

    // Camera

    auto viewport_height = 2.0;
    auto viewport_width  = aspect_ratio * viewport_height;
    auto focal_length    = 1.0;

    Camera cam;

    // Render
    PPMImage img(image_width, image_height);

    std::cerr << cam << std::endl;
    std::cerr << "Corner rays:\n\tl.l. " << cam.getRay(0, 0).direction
              << "\n\tt.l. " << cam.getRay(image_height, 0).direction
              << "\n\tl.r. " << cam.getRay(0, image_width).direction
              << "\n\tt.r. " << cam.getRay(image_height, image_width).direction << "\n";

    cam.render(world, img, samplesPerPixel);
    // #pragma omp parallel for // TODO: This went slower for some reason
    // for (int j = 0; j < image_height; ++j) {
    //     // std::cerr << "Row " << j << " / " << image_height << std::endl;
    //     for (int i = 0; i < image_width; ++i) {
    //         Color pxColor(0, 0, 0);
    //         for (int s = 0; s < samplesPerPixel; s++) {
    //             auto u = (i + randomDouble()) / (image_width - 1);
    //             auto v = (j + randomDouble()) / (image_height - 1);
    //             auto r = cam.getRay(u, v);
    //             pxColor += rayColor(r, world);
    //         }
    //         img.setPixel(i, j, pxColor * (1.0 / samplesPerPixel));
    //     }
    // }
    img.writeImage(std::cout);
}

// void renderScene(Camera& cam, HittableList& world)
