#pragma once
/// @file camera.hpp
///
/// The Camera class implements the perspective viewpoint from which the scene
/// is rendered.

#include "hittable.hpp"
#include "image.hpp"
#include "ray.hpp"
#include "rtweekend.hpp"

#include <ostream>

class Camera {
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
    Vec3 lookFrom = Vec3(0.0, 0.0, 1.0); ///< Point camera is looking from
    Vec3 lookAt   = Vec3(0.0, 0.0, 0.0); ///< Point camera is looking at
    Vec3 vup      = Vec3(0.0, 1.0, 0.0); ///< Camera relative Up

    /// @brief Variation angle of rays through each pixel
    double defocusAngle = 0;
    /// @brief Distance from camera lookFrom point to plane of perfect focus
    double focusDist    = 10.0;

    void render(const Hittable& world);
    /// @brief Get a ray for pixel (u,v), randomly sampled within the square
    /// covered by the pixel.
    /// @param u Horizontal position
    /// @param v Vertical position
    /// @param exact Set true to return exact direction instead of sampling
    /// within pixel square
    Ray getRay(double u, double v, bool exact = false) const;

private:
    void initialize();
    /// @brief Returns a random point in the square surrounding a pixel at the
    /// origin.
    Vec3 pixelSampleSquare() const;
    /// @brief Return a point offsetting ray origin within a unit disk.
    Vec3 defocusDiskSample() const;

    Vec3 viewportLowerLeft;
    /// @brief Image height (in pixels?)
    int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    Vec3 origin;        ///< Camera origin position
    Vec3 uPixelDelta;   ///< Offset to pixel in horizontal direction
    Vec3 vPixelDelta;   ///< Offset to pixel in vertical direction
    Vec3 pixel00Loc;    ///< Location of upper left pixel
    Vec3 u, v, w;       ///< Camera frame basis vectors
    Vec3 defocusDisk_u; ///< Defocus disk horizontal basis vector
    Vec3 defocusDisk_v; ///< Defocus disk vertical basis vector
};
