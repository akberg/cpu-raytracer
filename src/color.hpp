#pragma once
#include<ostream>

#include "rtweekend.hpp"

void writeColor(std::ostream& os, Color color) {
    os << static_cast<int>(255.999 * color.r) << ' '
       << static_cast<int>(255.999 * color.g) << ' '
       << static_cast<int>(255.999 * color.b) << '\n';
}
