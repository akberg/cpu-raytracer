#include "camera.hpp"
#include "image.hpp"

void Camera::render(const Hittable& world) {
    initialize();
    double sampleCoefficient = 1.0 / static_cast<double>(samplesPerPixel);
    // Tiling can greatly improve render time, but will be scene dependent and completely irrelevant
    // for BVH structures. Tesing uniti.tri (samples=16,depth=8, width=400) on Arm remote machine,
    // tile size:
    // - 1: 4272ms
    // - 4: 751ms
    // - 8: 354ms
    // - 16: 333ms
    int tileSize             = 16;
    for (int j = 0; j < img.height; j += tileSize) {
        for (int i = 0; i < img.width; i += tileSize) {
            for (int jt = 0; jt < tileSize && j + jt < img.height; jt++) {
                for (int it = 0; it < tileSize && i + it < img.width; it++) {
                    Color pxColor(0.0);
                    int x = i + it;
                    int y = j + jt;
                    for (int s = 0; s < samplesPerPixel; s++) {
                        auto u = (x + randomDouble()) / (img.width - 1);
                        auto v = (y + randomDouble()) / (img.height - 1);
                        auto r = getRay(x, y);
                        pxColor += rayColor(r, world, maxDepth);
                        // if (j % 40 == 0 && i % 20 == 0 && !s)
                        //     std::cerr << "(" << i << "," << j << ")" << r << " :
                        //     "
                        //               << pxColor << std::endl;
                    }
                    img.setPixel(x, y, pxColor * sampleCoefficient);
                }
            }
        }
        if (j % (img.height / 10) == 0) {
            std::cerr << "Ray tracing progress: " << j * 10 / (img.height / 10) << "%\n";
        }
    }
}

/// @brief Get a ray for pixel (u,v), randomly sampled within the square
/// covered by the pixel.
/// @param u Horizontal position
/// @param v Vertical position
/// @param exact Set true to return exact direction instead of sampling
/// within pixel square
Ray Camera::getRay(double u, double v, bool exact) const {
    auto pixelCenter = pixel00Loc + (u * uPixelDelta) + (v * vPixelDelta);
    auto pixelSample = pixelCenter + (exact ? Vec3(0.0) : pixelSampleSquare());
    auto rOrigin     = (defocusAngle <= 0) ? origin : defocusDiskSample();
    return Ray(rOrigin, pixelSample - rOrigin);
}

void Camera::initialize() {
    imageHeight = static_cast<int>(imageWidth / aspectRatio);
    imageHeight = (imageHeight < 1) ? 1 : imageHeight;

    // Resize image buffer
    if (img.width != imageWidth || img.height != imageHeight) {
        std::cerr << "Resize image (" << imageWidth << "," << imageHeight << ")\n";
        img.resize(imageWidth, imageHeight);
    }

    origin = lookFrom; // Why would we need to duplicate this???

    std::cerr << "Camera origin: " << origin << " look at: " << lookAt << "\n";

    // Calculate the u, v, w unit basis vectos for the camera coordinate
    // frame.
    w = glm::normalize(lookFrom - lookAt);
    u = glm::normalize(glm::cross(vup, w));
    v = glm::cross(w, u);

    // Determine viewport dimensions
    // // auto focalLength    = 1.0;
    auto theta          = degreesToRadians(vfov);
    auto h              = tan(theta / 2);
    auto viewportHeight = 2 * h * focusDist;
    auto viewportWidth  = viewportHeight * (static_cast<double>(imageWidth) / imageHeight);

    // Calculate the vectors across the horizontal and down the vertical
    // viewport edges. Edges go from lower left corner.
    auto uViewport = viewportWidth * u;
    auto vViewport = viewportHeight * v;

    // Calculate the horizontal and vertical delta vectors from pixel to
    // pixel.
    uPixelDelta = uViewport / static_cast<double>(imageWidth);
    vPixelDelta = vViewport / static_cast<double>(imageHeight);

    // Calculate the location of the upper left pixel.
    // sub focalLength, looking towards negative Z.
    // Place location in the middle of the pixel's covered area.
    viewportLowerLeft = origin - (focusDist * w) - uViewport / 2.0 - vViewport / 2.0;
    pixel00Loc        = viewportLowerLeft + 0.5 * (uPixelDelta + vPixelDelta);

    // Calculate camera defocus disk basis vectors.
    auto defocusRadius = focusDist * glm::tan(degreesToRadians(defocusAngle / 2.0));
    defocusDisk_u      = u * defocusRadius;
    defocusDisk_v      = v * defocusRadius;

    std::cerr << "Pixel (0,0) location: " << pixel00Loc << "\nuViewport: " << uViewport
              << " uPixelDelta: " << uPixelDelta << "\nvViewport: " << vViewport
              << " vPixelDelta: " << vPixelDelta << "\nu defocus disk: " << defocusDisk_u
              << " v defocus disk: " << defocusDisk_v << "\ndefocus radius: " << defocusRadius
              << "\n";
}

/// @brief Returns a random point in the square surrounding a pixel at the
/// origin.
Vec3 Camera::pixelSampleSquare() const {
    auto px = -0.5 + randomDouble();
    auto py = -0.5 + randomDouble();
    return (px * uPixelDelta) + (py * vPixelDelta);
}

/// @brief Return a point offsetting ray origin within a unit disk.
Vec3 Camera::defocusDiskSample() const {
    auto p = randomInUnitDisk();
    return origin + (p.x * defocusDisk_u) + (p.y + defocusDisk_v);
}