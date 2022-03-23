#pragma once

#include "headers.h"
#include "noise/perlin.h"
#include "vec3.h"

class noise_texture : public texture
{
public:
    perlin noise;

public:
    noise_texture() {}

    virtual color value(double u, double v, const point3 &p) const override
    {
        return color::identity() * noise.noise(p);
    }
};