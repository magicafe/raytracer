#pragma once

#include "headers.h"

struct hit_record;

class material
{
public:
    virtual color emitted(double u, double v, const point3 &p) const
    {
        return color::zero();
    }
    virtual bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const = 0;
};