#include "texture.hpp"

#include <cmath> // floor

Color SolidColorTexture::value(double u, double v, const Vec3& p) const {
    return colorVal;
}

Color CheckerTexture::value(double u, double v, const Vec3& p) const {
    auto xi = static_cast<int>(std::floor(invScale * p.x));
    auto yi = static_cast<int>(std::floor(invScale * p.y));
    auto zi = static_cast<int>(std::floor(invScale * p.z));

    bool evenSquare = (xi + yi + zi) % 2 == 0;

    return evenSquare ? even->value(u, v, p) : odd->value(u, v, p);
}
