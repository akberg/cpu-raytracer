#pragma once

#include "rtweekend.hpp"

#include <iostream>
#include <ostream>


static void writeColor(std::ostream& os, Color color)
{
    // gamma=2 correction
    float gamma = 1.25;
    auto r       = std::pow(color.r, 1.0 / gamma);
    auto g       = std::pow(color.g, 1.0 / gamma);
    auto b       = std::pow(color.b, 1.0 / gamma);

    os << static_cast<int>(256 * std::clamp(r, 0.0, 0.999)) << ' '
       << static_cast<int>(256 * std::clamp(g, 0.0, 0.999)) << ' '
       << static_cast<int>(256 * std::clamp(b, 0.0, 0.999)) << '\n';
}


static void writeColor(std::ostream& os, Color color, int samplesPerPixel)
{
    auto scale = 1.0 / samplesPerPixel;

    // gamma=2 correction
    float gamma = 1.25;
    auto r       = std::pow(color.r * scale, 1.0 / gamma);
    auto g       = std::pow(color.g * scale, 1.0 / gamma);
    auto b       = std::pow(color.b * scale, 1.0 / gamma);

    os << static_cast<int>(256 * std::clamp(r, 0.0, 0.999)) << ' '
       << static_cast<int>(256 * std::clamp(g, 0.0, 0.999)) << ' '
       << static_cast<int>(256 * std::clamp(b, 0.0, 0.999)) << '\n';
}
