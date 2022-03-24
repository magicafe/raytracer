#pragma once

#include "headers.h"
#include "material/material.h"
#include "texture/texture.h"
#include "texture/solid_color.h"

class diffuse_light : public material
{
public:
    shared_ptr<texture> emit;

public:
    diffuse_light(shared_ptr<textture> tex) : emit(tex) {}
    diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

    virtual bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        return false;
    }

    virtual color emitted(double u, double v, const point3 &p) const override
    {
        return emit->value(u, v, p);
    }
}