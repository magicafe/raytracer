#pragma once

#include <memory>
#include "hittable.h"
#include "vec3.h"
#include "material/material.h"

class sphere : public hittable
{
public:
    point3 center0;
    double radius;
    shared_ptr<material> mat_ptr;

public:
    sphere() {}
    sphere(point3 center, double r, shared_ptr<material> m) : center0(center), radius(r), mat_ptr(m) {}

    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
        output_box = aabb(center(0)  - vec3(radius, radius, radius), center(0) + vec3(radius, radius, radius));
        return true;
    }

protected:
    virtual point3 center(double time) const {
        return center0;
    }
};

bool
sphere::hit(const ray &r, double t_min, double t_max, hit_record &rec) const
{
    vec3 oc = r.origin() - center(r.time());
    auto a = r.direction().length_squared();
    auto half_b = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
    {
        return false;
    }

    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || root > t_max)
    {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || root > t_max)
        {
            return false;
        }
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center(r.time())) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;
    return true;
}