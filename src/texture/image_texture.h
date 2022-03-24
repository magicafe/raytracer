#pragma once

#include "headers.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "noise/perlin.h"

class image_texture : public texture
{
private:
    unsigned char *data;
    int width, height;
    int bytes_per_scanline;

public:
    const static int bytes_per_pixel = 3;

public:
    image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}
    image_texture(const char *filename)
    {
        auto components_per_pixel = bytes_per_pixel;
        data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);
        if (!data)
        {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = 0;
            height = 0;
        }
        bytes_per_scanline = bytes_per_pixel * width;
    }
    ~image_texture()
    {
        delete data;
    }

    virtual color value(double u, double v, const vec3 &p) const override
    {
        if (data == nullptr)
        {
            return color(1, 0, 0);
        }

        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        if (i >= width)
        {
            i = width - 1;
        }
        if (j >= height)
        {
            j = height - 1;
        }

        const auto color_scale = 1.0 / 255.0;
        auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

        return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }
};