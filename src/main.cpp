#include <iostream>

#include "Color.hpp"
#include "Ray.hpp"

double hitSphere(const Point& center, double radius, const Ray& r) {
    Vec3 oc = r.origin() - center;
    auto a = glm::dot(r.direction(), r.direction());
    auto half_b = glm::dot(oc, r.direction());
    auto c = glm::dot(oc, oc) - radius * radius;
    auto discr = half_b*half_b - a*c;

    if (discr < 0) {
        return -1.0;
    } else {
        return (-half_b - sqrt(discr) )/ (a);
    }
}

Color bgRayColor(const Ray& r) {
    Vec3 unit_direction = glm::normalize(r.direction());
    auto t = 0.5*(unit_direction.y + 1.0);
    return (1.0-t)*Color(1.0, 1.0, 1.0) + t*Color(0.5, 0.7, 1.0);
}

Color rayColor(const Ray& r) {
    auto t = hitSphere(Point(0, 0, -1), 0.5, r);

    if (t > 0.0) {
        Vec3 n = glm::normalize(r.at(t) - Vec3(0, 0, -1));
        return 0.5*Color(n.x+1, n.y+1, n.z+1);
    }

    return bgRayColor(r);
}



int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::cerr << "Unexpected argument: " << argv[i] << std::endl;
    }

    // Image

    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width   = 600;
    const int image_height  = static_cast<int>(image_width / aspect_ratio);

    // Camera

    auto viewport_height    = 2.0;
    auto viewport_width     = aspect_ratio * viewport_height;
    auto focal_length       = 1.0;

    auto origin     = Point(0, 0, 0);
    auto horizontal = Point(viewport_width, 0, 0);
    auto vertical   = Point(0, viewport_height, 0);
    auto lower_left_corner  = origin
        - horizontal / 2.0
        - vertical / 2.0
        - Vec3(0, 0, focal_length);

    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            auto u = double(i) / (image_width-1);
            auto v = double(j)/(image_height-1);
            auto direction = lower_left_corner + u*horizontal + v*vertical;
            Ray r(origin, direction);
            Color px_color = rayColor(r);
            // Color px_color(double(i)/(image_width-1), double(j)/(image_height-1), 0.25);
            writeColor(std::cout, px_color);
        }
    }
}