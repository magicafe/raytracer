#pragma once

#include "headers.h"
#include "noise/perlin.h"
#include "vec3.h"

class noise_texture : public texture
{
public:
    perlin noise;
    double scale;

public:
    noise_texture() {}
    noise_texture(double sc) : scale(sc) {}

    virtual color value(double u, double v, const point3 &p) const override
    {
        return color::identity() * 0.5 * (1 + sin(scale*p.z() + 10*noise.turb(p)));
    }
};