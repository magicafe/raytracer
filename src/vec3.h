#pragma once

#include "headers.h"
#include <iostream>
#include <Eigen/Dense>

using std::sqrt;

class vec3
{
public:
    // double e[3];
    Eigen::Vector3d e;

public:
    vec3() : e{0, 0, 0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}
    vec3(Eigen::Vector3d e) : e(e) {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e); }
    double operator[](int i) const { return e[i]; }
    double &operator[](int i) { return e[i]; }

    vec3 &operator+=(const vec3 &v)
    {
        e += v.e;
        return *this;
    }

    vec3 &operator*=(const double t)
    {
        e *= t;
        return *this;
    }

    vec3 &operator/=(const double t)
    {
        return *this *= (1 / t);
    }

    double length() const
    {
        return sqrt(length_squared());
    }

    double length_squared() const
    {
        return e.dot(e);
    }

    bool near_zero() const
    {
        // Return true if the vector is close to zero in all dimensions.
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

public:
    static vec3 zero() { return vec3(Eigen::Vector3d::Zero()); }

    static vec3 identity() { return vec3(1, 1, 1); }

    inline static vec3 random()
    {
        return vec3(random_double(), random_double(), random_double());
    }

    inline static vec3 random(double min, double max)
    {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

using point3 = vec3;
using color = vec3;

// Utility Functions
inline std::ostream &operator<<(std::ostream &out, const vec3 &v)
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v)
{
    return vec3(u.e + v.e);
}

inline vec3 operator+(const vec3 &u, double v)
{
    return vec3(u.e + v);
}

inline vec3 operator-(const vec3 &u, const vec3 &v)
{
    return vec3(u.e - v.e);
}

inline vec3 operator*(const vec3 &u, const vec3 &v)
{
    return vec3(u.e.cwiseProduct(v.e));
}

inline vec3 operator*(double t, const vec3 &v)
{
    return vec3(t * v.e);
}

inline vec3 operator*(const vec3 &u, double t)
{
    return t * u;
}

inline vec3 operator/(const vec3 &u, double t)
{
    return (1 / t) * u;
}

inline double dot(const vec3 &u, const vec3 &v)
{
    return u.e.dot(v.e);
}

inline vec3 cross(const vec3 &u, const vec3 &v)
{
    return vec3(u.e.cross(v.e));
}

inline vec3 reflect(const vec3 &v, const vec3 &n)
{
    return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3 &uv, const vec3 &n, double etai_over_etat)
{
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

inline vec3 unit_vector(vec3 v)
{
    return v / v.length();
}

inline vec3 random_in_unit_sphere()
{
    while (true)
    {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() >= 1)
        {
            continue;
        }
        return p;
    }
}

inline vec3 random_unit_vector()
{
    return unit_vector(random_in_unit_sphere());
}

inline vec3 random_in_hemisphere(const vec3 &normal)
{
    vec3 in_unit_shpere = random_in_unit_sphere();
    if (dot(in_unit_shpere, normal) > 0)
    {
        return in_unit_shpere;
    }
    else
    {
        return -in_unit_shpere;
    }
}

vec3 random_in_unit_disk()
{
    while (true)
    {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() >= 1)
        {
            continue;
        }
        return p;
    }
}