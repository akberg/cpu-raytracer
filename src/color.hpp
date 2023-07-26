#pragma once

#include "rtweekend.hpp"

#include <iostream>
#include<ostream>

void writeColor(std::ostream& os, Color color) {
    // gamma=2 correction
    double gamma = 1.25;
    auto r = std::pow(color.r, 1.0 / gamma);
    auto g = std::pow(color.g, 1.0 / gamma);
    auto b = std::pow(color.b, 1.0 / gamma);

    os << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
       << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
       << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
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

void writePPM(std::ostream& os, Color* img, int imageHeight, int imageWidth) {
    os << "P3\n"
       << imageWidth << ' ' << imageHeight << "\n255\n";

    for (int j = imageHeight - 1; j >= 0; --j) {
        for (int i = 0; i < imageWidth; ++i) {
            // Color px_color(double(i)/(image_width-1), double(j)/(image_height-1), 0.25);
            //std::cout << "("<<i<<","<<j<<")\n";// << img[imageWidth*j+i] << std::endl;
            // writeColor(os, img[imageWidth*j+i]);
        }
    }
}

class PPMImage {
public:
    PPMImage(int imageWidth, int imageHeight) : width(imageWidth), height(imageHeight) {
        image = new int[imageWidth*imageHeight];
    }
    ~PPMImage() {
        delete[] image;
    }

    int setPixel(int x, int y, Color color) {
        // auto scale = 1.0 / samplesPerPixel;
        if (color.r>1.0 || color.g>1.0 || color.b>1.0) {
            std::cerr << "Overflow: " << color << std::endl;
            return -EINVAL;
        }


        // gamma=2 correction
        double gamma = 1.25;
        auto rf = std::pow(color.r, 1.0 / gamma);
        auto gf = std::pow(color.g, 1.0 / gamma);
        auto bf = std::pow(color.b, 1.0 / gamma);

        auto r = static_cast<int>(256 * clamp(rf, 0.0, 0.999)) & 0xff;
        auto g = static_cast<int>(256 * clamp(gf, 0.0, 0.999)) & 0xff;
        auto b = static_cast<int>(256 * clamp(bf, 0.0, 0.999)) & 0xff;
        int  a = 0xff;

        image[y*width+x] = (r<<24) + (g<<16) + (b<<8) + (a<<0);
        return 0;
    }

    void writeImage(std::ostream& os) {
        int px, r, g, b;
        os << "P3\n"
           << width << ' ' << height << "\n255\n";
        for (int j = height - 1; j >= 0; --j) {
            for (int i = 0; i < width; ++i) {
                px = image[j*width+i];
                r = (px>>24) & 0xff;
                g = (px>>16) & 0xff;
                b = (px>> 8) & 0xff;
                os << r << " " << g << " " << b << "\n";
                // Color px_color(double(i)/(image_width-1), double(j)/(image_height-1), 0.25);
                //std::cout << "("<<i<<","<<j<<")\n";// << img[imageWidth*j+i] << std::endl;
                // writeColor(os, img[imageWidth*j+i]);
            }
        }
    }

private:
    int width;
    int height;
    int* image;
};
