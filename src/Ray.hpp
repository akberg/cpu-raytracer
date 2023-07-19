#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

using Point = glm::dvec3;
using Vec3 = glm::dvec3;

class Ray {
public:
    Ray() {};
    Ray(const Point& origin, const Vec3& direction)
        : _origin(origin), _direction(direction) {};
    Point origin() const { return _origin; };
    Vec3 direction() const { return _direction; };

    Point at(double t) const {
        return _origin + t * _direction;
    }

private:
    Point _origin;
    Vec3  _direction;
};
