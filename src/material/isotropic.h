#pragma once

#include "headers.h"
#include "material.h"
#include "texture/texture.h"
#include "texture/solid_color.h"

class isotropic : public material
{
public:
    shared_ptr<texture> albedo;

public:
    isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
    isotropic(shared_ptr<texture> tex) : albedo(tex) {}

    virtual bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }
};