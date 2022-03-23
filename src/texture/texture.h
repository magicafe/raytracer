#pragma once

#include "headers.h"

class texture
{
public:
    virtual color value(double u, double v, const point3 &p) const = 0;
};