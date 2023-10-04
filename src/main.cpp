#include "camera.hpp"
#include "hittableList.hpp"
#include "image.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "rtweekend.hpp"
#include "shape/plane.hpp"
#include "shape/sphere.hpp"
#include "shape/triangle.hpp"
#include "taskTimer.hpp"

#include <glm/glm.hpp>

#include <fstream>
#include <iostream>

const int N_MATERIALS                       = 9;
shared_ptr<Material> materials[N_MATERIALS] = {
    make_shared<Lambertian>(Color(0.1, 0.1, 0.1)),
    make_shared<Lambertian>(Color(1.0, 0.05, 0.05)),
    make_shared<Lambertian>(Color(0.05, 1.0, 0.05)),
    make_shared<Lambertian>(Color(0.05, 0.05, 1.0)),
    make_shared<Metal>(Color(1.0, 0.6, 0.6)),
    make_shared<Metal>(Color(1.0, 0.6, 0.6), 0.5),
    make_shared<Metal>(Color(0.5, 0.5, 0.5), 0.2),
    make_shared<Metal>(Color(0.99, 0.99, 0.5), 0.2),
    make_shared<Dielectric>(1.5),
};
auto matLambDark  = make_shared<Lambertian>(Color(0.1, 0.1, 0.1));
auto matLambRed   = make_shared<Lambertian>(Color(1.0, 0.05, 0.05));
auto matLambGreen = make_shared<Lambertian>(Color(0.05, 1.0, 0.05));
auto matLambBlue  = make_shared<Lambertian>(Color(0.05, 0.05, 1.0));
auto matMetRed    = make_shared<Metal>(Color(1.0, 0.6, 0.6));
auto matMetRed2   = make_shared<Metal>(Color(1.0, 0.6, 0.6), 0.5);
auto matMetDark   = make_shared<Metal>(Color(0.5, 0.5, 0.5), 0.2);
auto matMetLight  = make_shared<Metal>(Color(0.99, 0.99, 0.5), 0.2);
auto matRG        = make_shared<TwoSidedMaterial>(matMetDark, matLambGreen);
auto dielectric   = make_shared<Dielectric>(1.5);

HittableList spheres() {
    // World
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

    return world;
}

HittableList spheresAndTris() {
    HittableList world = spheres();
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

    return world;
}

std::vector<shared_ptr<Triangle>> triangles(int count) {
    auto tris      = std::vector<shared_ptr<Triangle>>(count);
    double maxPos  = 2.5;
    double maxEdge = 0.6;
    for (int i = 0; i < count; i++) {

        Point v0 = (Point(randomDouble(), randomDouble(), randomDouble()) - 0.5)
                 * maxPos;
        Point v1 = (Point(randomDouble(), randomDouble(), randomDouble()) - 0.5)
                 * maxEdge;
        Point v2 = (Point(randomDouble(), randomDouble(), randomDouble()) - 0.5)
                 * maxEdge;

        tris[i] = make_shared<Triangle>(
            v0,
            v0 + v1,
            v0 + v2,
            materials[randomInt(0, N_MATERIALS)]);
    }
    return tris;
}

int main(int argc, char* argv[]) {
    Point p(0.0, 5.0, 3.0);
    Point q(1.0, 2.0, 2.0);
    std::cerr << glm::min(p, q) << "\n";
    for (int i = 1; i < argc; i++) {
        std::cerr << "Unexpected argument: " << argv[i] << std::endl;
    }

    // New render
    Camera2 cam2;
    cam2.imageWidth      = 600;
    cam2.aspectRatio     = 16.0 / 9.0;
    cam2.samplesPerPixel = 50;
    cam2.lookAt          = Point(0.0, 0.0, -1.0);
    cam2.lookFrom        = Point(-0.6, 0.5, 0.9);

    HittableList world;

    //
    // world = spheresAndTris();
    // cam2.lookAt          = Point(0.0, 0.0, -1.0);
    // cam2.lookFrom        = Point(-1.0, 1.0, 0.9);

    // Triangles - - -
    cam2.lookAt   = Point(0.0, 0.0, -1.0);
    cam2.lookFrom = Point(-0.0, 0.0, 2.0);
    auto tris     = triangles(64);
    for (auto tri : tris) {
        world.add(tri);
    }

    TaskTimer tt;
    tt.start("Running ray tracing\n");
    cam2.render(world);
    tt.stop();
    std::ofstream f;
    f.open("runtime/triangles.ppm");
    if (!f.is_open()) std::cerr << "Failed to open file\n";
    cam2.img.writeImage(f);
    f.close();

    // img.writeImage(std::cout);
}
