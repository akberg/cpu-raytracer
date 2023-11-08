#pragma once

#include <material.hpp>
#include <rtweekend.hpp>
#include <shape/triangle.hpp>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::vector<shared_ptr<Triangle>>
loadTriFile(std::string filename, size_t length = 0) {
    // Fallback material for now. This model loading might be integrated in
    // some mesh class.
    auto fallbackMat = make_shared<Lambertian>(Color(0.82, 0.82, 0.82));

    std::vector<shared_ptr<Triangle>> tris;
    // if (length > 0) tris.resize(length);
    std::ifstream file(filename);
    std::string line;

    float a, b, c, d, e, f, g, h, i;
    size_t t = 0;
    std::getline(file, line);
    // std::cerr << line;

    while (0 != line.compare("999 999 999 999 999 999 999 999 999")) {
        sscanf(line.c_str(), "%f %f %f %f %f %f %f %f %f\n",
            &a, &b, &c, &d, &e, &f, &g, &h, &i );
        auto v0 = Vec3(a, b, c);
        auto v1 = Vec3(d, e, f);
        auto v2 = Vec3(g, h, i);
        tris.push_back(make_shared<Triangle>(v0, v1, v2, fallbackMat));
        std::getline(file, line);
    }
    std::cerr << "Loaded mesh of " << tris.size() << " tris from file "
              << filename << "\n";
    return tris;
}
