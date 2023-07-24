#pragma once

#include "rtweekend.hpp"

#include<ostream>

void writeColor(std::ostream& os, Color color) {
    os << static_cast<int>(255.999 * color.r) << ' '
       << static_cast<int>(255.999 * color.g) << ' '
       << static_cast<int>(255.999 * color.b) << '\n';
}

void writeColor(std::ostream& os, Color color, int samplesPerPixel) {
    auto scale = 1.0 / samplesPerPixel;

    // gamma=2 correction
    double gamma = 1.25;
    auto r = std::pow(color.r * scale, 1.0 / gamma);
    auto g = std::pow(color.g * scale, 1.0 / gamma);
    auto b = std::pow(color.b * scale, 1.0 / gamma);

    os << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
       << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
       << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}
