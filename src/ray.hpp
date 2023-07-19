#pragma once

#include "rtweekend.hpp"

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
