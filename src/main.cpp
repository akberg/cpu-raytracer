#include "camera.hpp"
#include "hittableList.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "image.hpp"
#include "rtweekend.hpp"
#include "shape/plane.hpp"
#include "shape/sphere.hpp"
#include "shape/triangle.hpp"

#include <glm/glm.hpp>

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::cerr << "Unexpected argument: " << argv[i] << std::endl;
    }

    // World
    auto matLambDark  = make_shared<Lambertian>(Color(0.1, 0.1, 0.1));
    auto matLambRed   = make_shared<Lambertian>(Color(1.0, 0.05, 0.05));
    auto matLambGreen = make_shared<Lambertian>(Color(0.05, 1.0, 0.05));
    auto matLambBlue  = make_shared<Lambertian>(Color(0.05, 0.05, 1.0));
    auto matMetRed    = make_shared<Metal>(Color(1.0, 0.6, 0.6));
    auto matMetRed2   = make_shared<Metal>(Color(1.0, 0.6, 0.6), 0.5);
    auto matMetDark   = make_shared<Metal>(Color(0.5, 0.5, 0.5), 0.2);
    auto matMetLight   = make_shared<Metal>(Color(0.99, 0.99, 0.5), 0.2);
    auto matRG        = make_shared<TwoSidedMaterial>(matMetDark, matLambGreen);
    auto dielectric   = make_shared<Dielectric>(1.5);
    HittableList world;
    // Visualise axes:
    // world.add(make_shared<Sphere>(Point( 0.5, -0.5, -2), 0.2, matLambRed));
    // world.add(make_shared<Sphere>(Point(-0.5,  0.5, -2), 0.2, matLambGreen));
    // world.add(make_shared<Sphere>(Point(-0.5, -0.5, -1), 0.2, matLambBlue));
    // world.add(make_shared<Sphere>(Point(-0.5, -0.5, -2), 0.2, matLambDark));

    world.add(make_shared<Sphere>(Point(-1, 0, -2), -0.5, dielectric));
    world.add(make_shared<Sphere>(Point(1, 0, -2), 0.5, matMetRed));
    // world.add(make_shared<Sphere>(Point(0, 0, -1), 0.5, matLambRed));
    world.add(make_shared<Sphere>(Point(0, 0, -2), 0.5, matMetRed2));

    world.add(make_shared<Sphere>(Point(10.0, 1.0, -4.0), 1.5, matLambBlue));
    world.add(make_shared<Sphere>(Point(1.0, 1.0, -6.0), 1.5, matMetRed2));
    world.add(make_shared<Sphere>(Point(-3.0, 1.0, -5.0), 1.5, matMetLight));

    world.add(make_shared<Sphere>(Point(0, -50.5, -2), 50.0, matMetDark));
    world.add(make_shared<Plane>(Point(0, -0.5, -4), Vec3(0, 1, 0), matRG));
    world.add(make_shared<Triangle>( // Cw pointing down
        Point(0.0, 0.2, -1.0),
        Point(2.0, 1.5, -1.0),
        Point(0.0, 1.0, -0.9),
        matRG));
    world.add(make_shared<Triangle>( // CW pointing down
        Point(1.0, 0.1, -1.0),
        Point(0.5, -0.3, -0.5),
        Point(0.2, 0.0, -0.8),
        matRG));
    world.add(make_shared<Triangle>( // CW pointing up
        Point(-0.2, 0.0, -0.8),
        Point(-1.0, 0.1, -1.0),
        Point(-0.5, -0.3, -0.5),
        matRG));
    world.add(make_shared<Triangle>( // CW pointing up
        Point(0.1, -0.5, 0.1),
        Point(1.0, 0.0, 0.5),
        Point(0.5, 0.0, 0.1),
        dielectric));


    // Image

    const auto aspectRatio     = 16.0 / 9.0;
    const int imageWidth       = 400;
    const int imageHeight      = static_cast<int>(imageWidth / aspectRatio);
    const int samplesPerPixel = 50;
    // Camera

    auto viewport_height = 2.0;
    auto viewport_width  = aspectRatio * viewport_height;
    auto focal_length    = 1.0;

    // New render
    Camera2 cam2;
    cam2.imageWidth = 400;
    cam2.aspectRatio = 16.0 / 9.0;
    cam2.samplesPerPixel = samplesPerPixel;
    cam2.lookAt = Point(0.0, 0.0, -1.0);
    cam2.lookFrom = Point(-0.75, 1.0, 1.0);
    cam2.render(world);
    std::ofstream f;
    f.open("runtime/image2.ppm");
    if (!f.is_open())
        std::cerr << "Failed to open file image2.ppm\n";
    cam2.img.writeImage(f);
    f.close();


    // Render
    // Camera cam;
    // PPMImage img(imageWidth, imageHeight);
    // cam.render(world, img, samplesPerPixel);
    // img.writeImage(std::cout);

}
