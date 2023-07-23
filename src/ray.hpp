#pragma once

#include <glm/glm.hpp>

#include "rtweekend.hpp"

class Ray {
public:
    Ray() {};
    Ray(const Point& origin, const Vec3& direction)
        : m_origin(origin), m_direction(glm::normalize(direction)) {};
    Point origin() const { return m_direction; };
    Vec3 direction() const { return m_direction; };

    Point at(double t) const {
        return m_origin + t * m_direction;
    }

private:
    Point m_origin;
    Vec3  m_direction;
};
