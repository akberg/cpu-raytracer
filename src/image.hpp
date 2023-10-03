#pragma once

#include "color.hpp"

#include <iostream>
#include <vector>

class Image {
public:
    virtual int setPixel(int x, int y, Color color) = 0;
    virtual void writeImage(std::ostream& os)       = 0;
};

class PPMImage : Image {
public:
    PPMImage() : width(0), height(0) { }
    PPMImage(int imageWidth, int imageHeight)
        : width(imageWidth)
        , height(imageHeight) {
        image.resize(imageWidth * imageHeight);
    }
    virtual ~PPMImage() { }

    void resize(int imageWidth, int imageHeight) {
        image.resize(imageWidth * imageHeight);
        width = imageWidth;
        height = imageHeight;
    }

    int setPixel(int x, int y, Color color) override {
        // auto scale = 1.0 / samplesPerPixel;
        if (color.r > 1.0 || color.g > 1.0 || color.b > 1.0) {
            std::cerr << "Overflow: " << color << std::endl;
            return -EINVAL;
        }

        // gamma=2 correction
        double gamma = 1.25;
        auto rf      = std::pow(color.r, 1.0 / gamma);
        auto gf      = std::pow(color.g, 1.0 / gamma);
        auto bf      = std::pow(color.b, 1.0 / gamma);

        auto r = static_cast<int>(256 * clamp(rf, 0.0, 0.999)) & 0xff;
        auto g = static_cast<int>(256 * clamp(gf, 0.0, 0.999)) & 0xff;
        auto b = static_cast<int>(256 * clamp(bf, 0.0, 0.999)) & 0xff;
        int a  = 0xff;

        image[y * width + x] = (r << 24) + (g << 16) + (b << 8) + (a << 0);
        return 0;
    }

    Color getPixel(int x, int y) const {
        int c = image[y * width + x];
        return Color(
            static_cast<double>((c >> 24) & 0xff) / 256.0,
            static_cast<double>((c >> 16) & 0xff) / 256.0,
            static_cast<double>((c >> 8) & 0xff) / 256.0
        );
    }

    void writeImage(std::ostream& os) override {
        int px, r, g, b;
        os << "P3\n" << width << ' ' << height << "\n255\n";
        for (int j = height - 1; j >= 0; --j) {
            for (int i = 0; i < width; ++i) {
                px = image[j * width + i];
                r  = (px >> 24) & 0xff;
                g  = (px >> 16) & 0xff;
                b  = (px >> 8) & 0xff;
                os << r << " " << g << " " << b << "\n";
            }
        }
    }

public:
    int width;
    int height;

private:
    std::vector<int> image;
};
