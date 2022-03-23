#pragma once

#include "headers.h"
#include "texture.h"
#include "solid_color.h"

class checker_texture : public texture
{
public:
    shared_ptr<texture> odd;
    shared_ptr<texture> even;

public:
    checker_texture() {}
    checker_texture(shared_ptr<texture> _odd, shared_ptr<texture> _even) : odd(_odd), even(_even) {}
    checker_texture(color c1, color c2) : odd(make_shared<solid_color>(c1)),even(make_shared<solid_color>(c2)) {}

    virtual color value(double u, double v, const point3 &p) const override
    {
        auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0)
        {
            return odd->value(u, v, p);
        }
        else
        {
            return even->value(u, v, p);
        }
    }
};