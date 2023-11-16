#include "camera.hpp"
#include "image.hpp"

void Camera::render(const Hittable& world) {
    initialize();
    double sampleCoefficient = 1.0 / static_cast<double>(samplesPerPixel);
    for (int j = 0; j < img.height; j++) {
        for (int i = 0; i < img.width; i++) {
            Color pxColor(0.0);
            for (int s = 0; s < samplesPerPixel; s++) {
                auto u = (i + randomDouble()) / (img.width - 1);
                auto v = (j + randomDouble()) / (img.height - 1);
                auto r = getRay(i, j);
                pxColor += rayColor(r, world, maxDepth);
                // if (j % 40 == 0 && i % 20 == 0 && !s)
                //     std::cerr << "(" << i << "," << j << ")" << r << " :
                //     "
                //               << pxColor << std::endl;
            }
            img.setPixel(i, j, pxColor * sampleCoefficient);
        }
        if (j % (img.height / 10) == 0) {
            std::cerr << "Ray tracing progress: "
                        << j * 10 / (img.height / 10) << "%\n";
        }
    }
}

/// @brief Get a ray for pixel (u,v), randomly sampled within the square
/// covered by the pixel.
/// @param u Horizontal position
/// @param v Vertical position
/// @param exact Set true to return exact direction instead of sampling
/// within pixel square
Ray Camera::getRay(double u, double v, bool exact = false) const {
    auto pixelCenter = pixel00Loc + (u * uPixelDelta) + (v * vPixelDelta);
    auto pixelSample =
        pixelCenter + (exact ? Vec3(0.0) : pixelSampleSquare());
    auto rOrigin = (defocusAngle <= 0) ? origin : defocusDiskSample();
    return Ray(rOrigin, pixelSample - rOrigin);
}

void Camera::initialize() {
    imageHeight = static_cast<int>(imageWidth / aspectRatio);
    imageHeight = (imageHeight < 1) ? 1 : imageHeight;

    // Resize image buffer
    if (img.width != imageWidth || img.height != imageHeight) {
        std::cerr << "Resize image (" << imageWidth << "," << imageHeight
                    << ")\n";
        img.resize(imageWidth, imageHeight);
    }

    origin = lookFrom; // Why would we need to duplicate this???

    std::cerr << "Camera origin: " << origin << " look at: " << lookAt
                << "\n";

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
    auto viewportWidth =
        viewportHeight * (static_cast<double>(imageWidth) / imageHeight);

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
    viewportLowerLeft =
        origin - (focusDist * w) - uViewport / 2.0 - vViewport / 2.0;
    pixel00Loc = viewportLowerLeft + 0.5 * (uPixelDelta + vPixelDelta);

    // Calculate camera defocus disk basis vectors.
    auto defocusRadius =
        focusDist * glm::tan(degreesToRadians(defocusAngle / 2.0));
    defocusDisk_u = u * defocusRadius;
    defocusDisk_v = v * defocusRadius;

    std::cerr << "Pixel (0,0) location: " << pixel00Loc
                << "\nuViewport: " << uViewport
                << " uPixelDelta: " << uPixelDelta
                << "\nvViewport: " << vViewport
                << " vPixelDelta: " << vPixelDelta
                << "\nu defocus disk: " << defocusDisk_u
                << " v defocus disk: " << defocusDisk_v
                << "\ndefocus radius: " << defocusRadius << "\n";
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