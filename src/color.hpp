#pragma once
#include<ostream>

#include "rtweekend.hpp"

void writeColor(std::ostream& os, Color color) {
    os << static_cast<int>(255.999 * color.r) << ' '
       << static_cast<int>(255.999 * color.g) << ' '
       << static_cast<int>(255.999 * color.b) << '\n';
}

void writeColor(std::ostream& os, Color color, int samplesPerPixel) {
    auto scale = 1.0 / samplesPerPixel;

    auto r = sqrt(color.r * scale);
    auto g = sqrt(color.g * scale);
    auto b = sqrt(color.b * scale);

    os << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
       << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
       << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}
