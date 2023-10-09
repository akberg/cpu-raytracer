#pragma once
/// @file camera.hpp
///
/// The Camera class implements the perspective viewpoint from which the scene
/// is rendered.

#include "color.hpp"
#include "hittable.hpp"
#include "image.hpp"
#include "ray.hpp"
#include "rtweekend.hpp"

#include <ostream>

class Camera {
public:
    Camera() : Camera(16.0 / 9.0, 1.0) { }

    Camera(double aspectRatio, double focalLength);

    // TODO : help

    /// @brief Set aspect ratio. Updates dependent values.
    /// @param aspect Aspect ratio
    void setAspectRatio(double aspect) {
        this->aspectRatio = aspect;
        horizontal        = Point(aspect * vertical.y, 0, 0);
        lowerLeftCorner   = origin - horizontal / 2.0 - vertical / 2.0
                        - Vec3(0, 0, focalLength);
    }

    friend std::ostream& operator<<(std::ostream& os, Camera cam) {
        os << "Camera(o:" << cam.origin
           << " direction: " << cam.getRay(0.5, 0.5).direction << ")";
        return os;
    }

    void render(Hittable& world, PPMImage& img, int samplesPerPixel);

    /// @brief Origin of lens
    Point origin;
    /// @brief Focal Lenght affects the spreading angle of cast rays, and
    /// thereby the field of view. Lower focal length makes a wider angle, and
    /// a longer focal length makes a narrower angle.
    double focalLength = 1.0;
    /// @brief Ratio of width over height
    double aspectRatio = 1.0;
    int imageWidth     = 400;

private:
    Point horizontal;
    Point vertical;
    Point lowerLeftCorner;

    /// @brief Get ray for the UV direction
    /// @param u Horizontal proportion, in [0, 1)
    /// @param v Vertical proportion, in [0, 1)
    /// @return Cast ray
    Ray getRay(double u, double v) const;

    void initialize();
};

class Camera2 {
public:
    PPMImage img;
    // Quality and Performance - - -
    int samplesPerPixel = 10;
    int maxDepth        = 10;
    int imageWidth      = 100;

    // Image settings - - -
    double aspectRatio = 1.0;
    /// @brief Vertical FoV angle
    double vfov        = 90;

    // Camera settings - - -
    Point lookFrom = Vec3(0.0, 0.0, 1.0); ///< Point camera is looking from
    Point lookAt   = Vec3(0.0, 0.0, 0.0); ///< Point camera is looking at
    Vec3 vup       = Vec3(0.0, 1.0, 0.0); ///< Camera relative Up

    /// @brief Variation angle of rays through each pixel
    double defocusAngle = 0;
    /// @brief Distance from camera lookFrom point to plane of perfect focus
    double focusDist    = 10.0;

    void render(const Hittable& world) {
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
    /// @param exact Set true to return exact direction instead of sampling within pixel square
    Ray getRay(double u, double v, bool exact = false) const {
        auto pixelCenter = pixel00Loc + (u * uPixelDelta) + (v * vPixelDelta);
        auto pixelSample =
            pixelCenter + (exact ? Vec3(0.0) : pixelSampleSquare());
        auto rOrigin = (defocusAngle <= 0) ? origin : defocusDiskSample();
        return Ray(rOrigin, pixelSample - rOrigin);
    }

private:
    void initialize() {
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
    Vec3 pixelSampleSquare() const {
        auto px = -0.5 + randomDouble();
        auto py = -0.5 + randomDouble();
        return (px * uPixelDelta) + (py * vPixelDelta);
    }

    /// @brief Return a point offsetting ray origin within a unit disk.
    Vec3 defocusDiskSample() const {
        auto p = randomInUnitDisk();
        return origin + (p.x * defocusDisk_u) + (p.y + defocusDisk_v);
    }

    Vec3 viewportLowerLeft;
    /// @brief Image height (in pixels?)
    int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    Vec3 origin;     ///< Camera origin position
    Vec3 uPixelDelta; ///< Offset to pixel in horizontal direction
    Vec3 vPixelDelta; ///< Offset to pixel in vertical direction
    Vec3 pixel00Loc; ///< Location of upper left pixel
    Vec3 u, v, w;     ///< Camera frame basis vectors
    Vec3 defocusDisk_u; ///< Defocus disk horizontal basis vector
    Vec3 defocusDisk_v; ///< Defocus disk vertical basis vector
};
