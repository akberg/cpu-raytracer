/// @file texture/texture.hpp
/// Defines Texture interface for supporting texture mapping.
#pragma once
#include "image.hpp" // STBImage for ImageTexture
#include "rtweekend.hpp"

#include <algorithm> // clamp for ImageTexture

class Texture {
public:
    virtual ~Texture() = default;

    virtual Color value(float u, float v, const Vec3& p) const = 0;
};

// ----------------------------------------------------------------------------/

/// @brief Texture containing just a solid colour.
class SolidColorTexture : public Texture {
public:
    SolidColorTexture(Color c) : colorVal(c) { }
    SolidColorTexture(float red, float green, float blue)
        : colorVal(red, green, blue) { }

    Color value(float u, float v, const Vec3& p) const override;

private:
    Color colorVal;
};

/// @brief Checker pattern texture.
class CheckerTexture : public Texture {
public:
    /// @brief Create a checker pattern texture
    /// @param scale Scale of checker fields
    /// @param even Texture in even checker fields
    /// @param odd Texture in odd checker fields
    CheckerTexture(
        float scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
        : invScale(1.0f / scale)
        , even(even)
        , odd(odd) { }
    /// @brief Create a checker pattern texture
    /// @param scale Scale of checker fields
    /// @param even Color of a solid color texture in even checker fields
    /// @param odd Color of a solid color texture in odd checker fields
    CheckerTexture(float scale, Color even, Color odd)
        : invScale(1.0f / scale)
        , even(make_shared<SolidColorTexture>(even))
        , odd(make_shared<SolidColorTexture>(odd)) { }

    Color value(float u, float v, const Vec3& p) const override;

private:
    float invScale = 1.0;
    shared_ptr<Texture> even;
    shared_ptr<Texture> odd;
};

class GradientTexture : public Texture {
public:
    GradientTexture(
        shared_ptr<Texture> texture0,
        shared_ptr<Texture> texture1,
        bool vertical,
        bool horizontal)
        : vert(vertical)
        , hori(horizontal)
        , t0(texture0)
        , t1(texture1) { }
    GradientTexture(
        const Color& texture0,
        const Color& texture1,
        bool vertical,
        bool horizontal)
        : vert(vertical)
        , hori(horizontal)
        , t0(make_shared<SolidColorTexture>(texture0))
        , t1(make_shared<SolidColorTexture>(texture1)) { }

    Color value(float u, float v, const Vec3& p) const override {
        Color c0 = t0->value(u, v, p);
        Color c1 = t1->value(u, v, p);

        u = std::clamp(u, 0.0f, 1.0f);
        v = std::clamp(v, 0.0f, 1.0f);

        float grad = 0.0f;
        if (vert && hori) {
            grad = u * v;
        } else if (vert) {
            grad = u;
        } else if (hori) {
            grad = v;
        }

        return c1 * grad + c0 * (1.0f - grad);
    }

private:
    bool vert, hori;
    shared_ptr<Texture> t0;
    shared_ptr<Texture> t1;
};

class ImageTexture : public Texture {
public:
    float repeatedTexture = 1.0f;
public:
    ImageTexture(const std::string& filename) : image(filename) { }
    ImageTexture(STBImage& image) : image(image) { }

    Color value(float u, float v, const Vec3& p) const override {
        // For debugging, return cyan if there is no image.
        if (image.height() <= 0) return Color(0.0f, 1.0f, 1.0f);

        // Here depends on multiple image settings.
        // - repeated texture? Then do modulo instead.
        u = std::clamp(u, 0.0f, 1.0f);// * repeatedTexture;
        v = 1.0 - std::clamp(v, 0.0f, 1.0f);

        auto i = static_cast<int>(u * image.width());// % image.width();
        auto j = static_cast<int>(v * image.height());

        return image.getPixel(i, j);
    }

private:
    STBImage image;
};
