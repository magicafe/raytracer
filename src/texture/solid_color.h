#pragma once

#include "headers.h"
#include "texture.h"

class solid_color : public texture
{
private:
    color color_value;

public:
    solid_color() {}
    solid_color(color c) : color_value(c) {}
    solid_color(double red, double blue, double green) : color_value(color(red, blue, green)) {}

    virtual color value(double u, double v, const point3& p) const override {
        return color_value;
    }
};