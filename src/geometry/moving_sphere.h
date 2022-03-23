#pragma once

#include "sphere.h"
#include "headers.h"

class moving_sphere : public sphere
{
public:
    point3 center1;
    double time0, time1;

public:
    moving_sphere() {}
    moving_sphere(point3 cen0, point3 cen1, double time0, double time1, double r, shared_ptr<material> m)
        : sphere(cen0, r, m), center1(cen1), time0(time0), time1(time1) {}

protected:
    virtual point3 center(double time) const override {
        return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }
};
