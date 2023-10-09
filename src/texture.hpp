/// @file texture/texture.hpp
/// Defines Texture interface for supporting texture mapping.
#pragma once
#include "rtweekend.hpp"

class Texture {
public:
    virtual ~Texture() = default;

    virtual Color value(double u, double v, const Vec3& p) const = 0;
};

// ----------------------------------------------------------------------------/

/// @brief Texture containing just a solid colour.
class SolidColorTexture : public Texture {
public:
    SolidColorTexture(Color c) : colorVal(c) { }
    SolidColorTexture(double red, double green, double blue)
        : colorVal(red, green, blue) { }

    Color value(double u, double v, const Vec3& p) const override;

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
        double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
        : invScale(1.0 / scale)
        , even(even)
        , odd(odd) { }
    /// @brief Create a checker pattern texture
    /// @param scale Scale of checker fields
    /// @param even Color of a solid color texture in even checker fields
    /// @param odd Color of a solid color texture in odd checker fields
    CheckerTexture(double scale, Color even, Color odd)
        : invScale(1.0 / scale)
        , even(make_shared<SolidColorTexture>(even))
        , odd(make_shared<SolidColorTexture>(odd)) { }

    Color value(double u, double v, const Vec3& p) const override;
private:
    double invScale = 1.0;
    shared_ptr<Texture> even;
    shared_ptr<Texture> odd;
};
