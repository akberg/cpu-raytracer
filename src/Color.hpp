#pragma once
#include<glm/glm.hpp>
#include<ostream>

typedef glm::dvec3 Color;

void writeColor(std::ostream& os, Color color) {
    os << static_cast<int>(255.999 * color.r) << ' '
       << static_cast<int>(255.999 * color.g) << ' '
       << static_cast<int>(255.999 * color.b) << '\n';
}
