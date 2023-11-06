#include "acceleration/bvh.hpp"
#include "camera.hpp"
#include "hittableList.hpp"
#include "image.hpp"
#include "material.hpp"
#include "modelTri.hpp"
#include "ray.hpp"
#include "rtweekend.hpp"
#include "shape/plane.hpp"
#include "shape/quad.hpp"
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
    // world.add(make_shared<Sphere>(Vec3( 0.5, -0.5, -2), 0.2, matLambRed));
    // world.add(make_shared<Sphere>(Vec3(-0.5,  0.5, -2), 0.2, matLambGreen));
    // world.add(make_shared<Sphere>(Vec3(-0.5, -0.5, -1), 0.2, matLambBlue));
    // world.add(make_shared<Sphere>(Vec3(-0.5, -0.5, -2), 0.2, matLambDark));

    world.add(make_shared<Sphere>(Vec3(-1, 0, -2), -0.5, dielectric));
    world.add(make_shared<Sphere>(Vec3(1, 0, -2), 0.5, matMetRed));
    // world.add(make_shared<Sphere>(Vec3(0, 0, -1), 0.5, matLambRed));
    world.add(make_shared<Sphere>(Vec3(0, 0, -2), 0.5, matMetRed2));

    world.add(make_shared<Sphere>(Vec3(10.0, 1.0, -4.0), 1.5, matLambBlue));
    world.add(make_shared<Sphere>(Vec3(1.0, 1.0, -6.0), 1.5, matMetRed2));
    world.add(make_shared<Sphere>(Vec3(-3.0, 1.0, -5.0), 1.5, matMetLight));

    world.add(make_shared<Sphere>(Vec3(0, -50.5, -2), 50.0, matMetDark));
    world.add(make_shared<Plane>(Vec3(0, -0.5, -4), Vec3(0, 1, 0), matRG));

    return world;
}

HittableList spheresAndTris() {
    HittableList world = spheres();
    world.add(make_shared<Triangle>( // Cw pointing down
        Vec3(0.0, 0.2, -1.0),
        Vec3(2.0, 1.5, -1.0),
        Vec3(0.0, 1.0, -0.9),
        matRG));
    world.add(make_shared<Triangle>( // CW pointing down
        Vec3(1.0, 0.1, -1.0),
        Vec3(0.5, -0.3, -0.5),
        Vec3(0.2, 0.0, -0.8),
        matRG));
    world.add(make_shared<Triangle>( // CW pointing up
        Vec3(-0.2, 0.0, -0.8),
        Vec3(-1.0, 0.1, -1.0),
        Vec3(-0.5, -0.3, -0.5),
        matRG));
    world.add(make_shared<Triangle>( // CW pointing up
        Vec3(0.1, -0.5, 0.1),
        Vec3(1.0, 0.0, 0.5),
        Vec3(0.5, 0.0, 0.1),
        dielectric));

    return world;
}

std::vector<shared_ptr<Triangle>> triangles(int count) {
    auto tris      = std::vector<shared_ptr<Triangle>>(count);
    double maxPos  = 2.5;
    double maxEdge = 0.5;
    for (int i = 0; i < count; i++) {

        Vec3 v0 = (Vec3(randomDouble(), randomDouble(), randomDouble()) - 0.5)
                * maxPos * 2.0;
        Vec3 v1 = (Vec3(randomDouble(), randomDouble(), randomDouble()) - 0.5)
                * maxEdge * 2.0;
        Vec3 v2 = (Vec3(randomDouble(), randomDouble(), randomDouble()) - 0.5)
                * maxEdge * 2.0;

        tris[i] = make_shared<Triangle>(
            v0,
            v0 + v1,
            v0 + v2,
            materials[randomInt(0, N_MATERIALS)]);
    }
    return tris;
}

std::vector<shared_ptr<Triangle>> box() {
    Vec3 center = Vec3(0.0);
    Vec3 s      = Vec3(2.0);
    auto mat    = matLambRed;

    Vec3 v[] = {
        center + Vec3(s.x, s.y, s.z),    // 0
        center + Vec3(s.x, s.y, -s.z),   // 1
        center + Vec3(s.x, -s.y, s.z),   // 2
        center + Vec3(s.x, -s.y, -s.z),  // 3
        center + Vec3(-s.x, s.y, s.z),   // 4
        center + Vec3(-s.x, s.y, -s.z),  // 5
        center + Vec3(-s.x, -s.y, s.z),  // 6
        center + Vec3(-s.x, -s.y, -s.z), // 7
    };

    auto tris = std::vector<shared_ptr<Triangle>>({
        make_shared<Triangle>(v[0], v[1], v[3], mat),
        make_shared<Triangle>(v[0], v[3], v[2], mat),
        make_shared<Triangle>(v[0], v[4], v[5], mat),
        make_shared<Triangle>(v[0], v[5], v[1], mat),
        make_shared<Triangle>(v[0], v[6], v[4], mat),
        make_shared<Triangle>(v[0], v[2], v[6], mat),
        make_shared<Triangle>(v[7], v[5], v[4], mat),
        make_shared<Triangle>(v[7], v[4], v[6], mat),
        make_shared<Triangle>(v[7], v[6], v[2], mat),
        make_shared<Triangle>(v[7], v[2], v[3], mat),
        make_shared<Triangle>(v[7], v[3], v[1], mat),
        make_shared<Triangle>(v[7], v[1], v[5], mat),
    });
    return tris;
}

void renderOneBox() {
    TaskTimer tt;
    std::ofstream f;

    // New render
    Camera2 cam;
    cam.imageWidth      = 300;
    cam.aspectRatio     = 16.0 / 9.0;
    cam.samplesPerPixel = 100;
    cam.lookAt          = Vec3(0.0, 0.0, 3.0);
    cam.lookFrom        = Vec3(0.0, 0.0, 4.0);
    cam.defocusAngle    = 0.000001;

    auto tris = box();
    BVH world(tris);
    tt.start("BVH render box . . .\n");
    cam.render(world);
    tt.stop();

    std::string filename = "runtime/cube.ppm";
    f.open(filename);
    if (!f.is_open()) std::cerr << "Failed to open file: " << filename << "\n";
    cam.img.writeImage(f);
    f.close();
}

void renderOneSphere() {
    TaskTimer tt;
    std::ofstream f;

    // New render
    Camera2 cam;
    cam.imageWidth      = 300;
    cam.aspectRatio     = 16.0 / 9.0;
    cam.vfov            = 90;
    cam.samplesPerPixel = 100;
    cam.lookAt          = Vec3(0.0, 0.0, 0.0);
    cam.lookFrom        = Vec3(0.0, 0.0, 2.5);
    cam.focusDist       = 2.0;
    cam.defocusAngle    = 0.000001;

    auto mat1 = make_shared<Metal>(Color(0.2, 0.2, 1.0), 0.1);

    auto s = Sphere(Vec3(0.0), 1.0, mat1);
    tt.start("Render sphere . . .\n");
    cam.render(s);
    tt.stop();

    std::string filename = "runtime/sphere.ppm";
    f.open(filename);
    if (!f.is_open()) std::cerr << "Failed to open file: " << filename << "\n";
    cam.img.writeImage(f);
    f.close();
}

void renderEarth(double hour = 0.0) {
    TaskTimer tt;
    std::ofstream f;

    // New render
    Camera2 cam;
    cam.imageWidth      = 800;
    cam.aspectRatio     = 16.0 / 9.0;
    cam.vfov            = 90;
    cam.samplesPerPixel = 100;
    cam.lookAt          = Vec3(0.0, 0.0, 0.0);
    cam.lookFrom        = Vec3(0.0, 1.0, -3.0);
    cam.focusDist       = 1.5;
    cam.defocusAngle    = 0.0;

    auto imgTexEarth = make_shared<ImageTexture>("resources/earthmap.jpg");
    auto matEarthmap = make_shared<Lambertian>(imgTexEarth);
    auto earth       = Sphere(Vec3(0.0, 0.0, 0.0), 2.0, matEarthmap);

    tt.start("Render spheres . . .\n");
    cam.render(earth);
    tt.stop();

    std::string filename = "runtime/earth.ppm";
    f.open(filename);
    if (!f.is_open()) std::cerr << "Failed to open file: " << filename << "\n";
    cam.img.writeImage(f);
    f.close();
}

void renderTwoSpheres() {
    TaskTimer tt;
    std::ofstream f;

    // New render
    Camera2 cam;
    cam.imageWidth      = 600;
    cam.aspectRatio     = 16.0 / 9.0;
    cam.vfov            = 80;
    cam.samplesPerPixel = 100;
    cam.lookAt          = Vec3(-1.25, 0.0, 0.0);
    cam.lookFrom        = Vec3(-1.25, 2.0, 0.1);
    cam.focusDist       = 1.5;
    cam.defocusAngle    = 0.0;

    // STBImage tex("resources/cgaxis_hdri_skies_01_52.jpg");
    auto imgTex =
        make_shared<ImageTexture>("resources/cgaxis_hdri_skies_01_52.jpg");
    auto imgTexEarth = make_shared<ImageTexture>("resources/earthmap.jpg");
    imgTex->repeatedTexture = 1.0;

    auto checker =
        make_shared<CheckerTexture>(0.025, Color(1.0), Color(0.0, 0.0, 1.0));
    auto grad = make_shared<GradientTexture>(
        Color(1.0, 0.0, 0.0),
        Color(0.0, 0.0, 1.0),
        true,
        false);
    auto mat1 = make_shared<Metal>(checker, 0.1);
    // auto mat2 = make_shared<Metal>(Color(0.95, 0.95, 0.95), 0.6);
    auto mat2 = make_shared<Lambertian>(imgTexEarth);
    // auto mat2 = make_shared<Lambertian>(grad);

    HittableList world;

    world.add(make_shared<Sphere>(Vec3(1.25, 0.0, 0.0), 1.0, mat1));
    world.add(make_shared<Sphere>(Vec3(-1.25, 0.0, 0.0), 1.0, mat2));
    tt.start("Render spheres . . .\n");
    cam.render(world);
    tt.stop();

    std::string filename = "runtime/2spheres.ppm";
    f.open(filename);
    if (!f.is_open()) std::cerr << "Failed to open file: " << filename << "\n";
    cam.img.writeImage(f);
    f.close();
}

void renderQuads() {
    TaskTimer tt;
    std::ofstream f;

    // New render
    Camera2 cam;
    cam.imageWidth      = 400;
    cam.aspectRatio     = 1.0;
    cam.vfov            = 80;
    cam.samplesPerPixel = 100;
    cam.lookAt          = Vec3(0.0, 0.0, 0.0);
    cam.lookFrom        = Vec3(0.0, 0.0, 9.0);
    cam.vup             = Vec3(0.0, 1.0, 0.0);
    cam.focusDist       = 1.5;
    cam.defocusAngle    = 0.0;

    auto leftRed     = make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
    auto backGreen   = make_shared<Lambertian>(Color(0.2, 1.0, 0.2));
    auto rightBlue   = make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
    auto upperOrange = make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
    auto lowerTeal   = make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

    HittableList world;
    world.add(make_shared<Quad>(Vec3(-3, -2, 5), Vec3(0, 0, -4), Vec3(0, 4, 0), leftRed));
    world.add(make_shared<Quad>(Vec3(-2, -2, 0), Vec3(4, 0, 0), Vec3(0, 4, 0), backGreen));
    world.add(make_shared<Quad>(Vec3(3, -2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), rightBlue));
    world.add(make_shared<Quad>(Vec3(-2, 3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), upperOrange));
    world.add(make_shared<Quad>(Vec3(-2, -3, 5), Vec3(4, 0, 0), Vec3(0, 0, -4), lowerTeal));
    tt.start("Render quads . . .\n");
    cam.render(world);
    tt.stop();

    std::string filename = "runtime/quads.ppm";
    f.open(filename);
    if (!f.is_open()) std::cerr << "Failed to open file: " << filename << "\n";
    cam.img.writeImage(f);
    f.close();
}

void renderTriangles(int nTris = 32) {
    TaskTimer tt;
    std::ofstream f;

    // New render
    Camera2 cam;
    cam.imageWidth      = 400;
    cam.aspectRatio     = 1.0;
    cam.vfov            = 80;
    cam.samplesPerPixel = 100;
    cam.lookAt          = Vec3(0.0, 0.0, 0.0);
    cam.lookFrom        = Vec3(0.0, 0.0, 9.0);
    cam.vup             = Vec3(0.0, 1.0, 0.0);
    cam.focusDist       = 1.5;
    cam.defocusAngle    = 0.0;

    auto tris = triangles(nTris);

    BVH world(tris);

    auto s = std::format("Render {} triangles . . .\n", nTris);
    tt.start(s);
    cam.render(world);
    tt.stop();

    std::string filename = std::format("runtime/triangles-{}pc.ppm", nTris);
    f.open(filename);
    if (!f.is_open()) std::cerr << "Failed to open file: " << filename << "\n";
    cam.img.writeImage(f);
    f.close();
}

void renderUnityMesh() {
    TaskTimer tt;
    std::ofstream f;

    // New render
    Camera2 cam;
    cam.imageWidth      = 400;
    cam.aspectRatio     = 1.0;
    cam.vfov            = 80;
    cam.samplesPerPixel = 100;
    cam.lookAt          = Vec3(0.0, 0.0, 0.0);
    cam.lookFrom        = Vec3(0.0, 0.0, 9.0);
    cam.vup             = Vec3(0.0, 1.0, 0.0);
    cam.focusDist       = 1.5;
    cam.defocusAngle    = 0.0;

    auto tris = loadTriFile("resources/unity.tri");

    BVH world(tris);
    tt.start("Render unity.tri mesh . . .\n");
    cam.render(world);
    tt.stop();

    std::string filename = "runtime/unityMesh.ppm";
    f.open(filename);
    if (!f.is_open()) std::cerr << "Failed to open file: " << filename << "\n";
    cam.img.writeImage(f);
    f.close();
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::cerr << "Unexpected argument: " << argv[i] << std::endl;
    }

    std::cout << "A BVH node currently requires " << sizeof(BVHNode) << " bytes.\n";
    // renderEarth();
    // renderQuads(); // Best time 21645ms
    // renderOneBox(); // Best time 7804ms
    // renderUnityMesh();
    renderTriangles(178); // 178 tris best time 8351ms
    renderTriangles(512); // 512 tris best time 14183ms
}
