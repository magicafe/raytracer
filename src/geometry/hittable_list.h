#pragma once

#include "hittable.h"
#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

class hittable_list : public hittable
{
public:
    std::vector<shared_ptr<hittable>> objects;

public:
    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void add(shared_ptr<hittable> object) { objects.push_back(object); }
    void clear() { objects.clear(); }

    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb &output_box) const override
    {
        if (objects.empty())
        {
            return false;
        }

        aabb temp_box;
        bool fisrt_box = true;

        for (auto &object : objects)
        {
            if (!object->bounding_box(time0, time1, temp_box))
            {
                return false;
            }

            output_box = fisrt_box ? temp_box : surrounding_box(output_box, temp_box);
            fisrt_box = false;
        }

        return true;
    }
};

bool hittable_list::hit(const ray &r, double t_min, double t_max, hit_record &rec) const
{
    hit_record temp_rec;
    bool hit_anything = false;
    auto closet_so_far = t_max;

    for (const auto &object : objects)
    {
        if (object->hit(r, t_min, closet_so_far, temp_rec))
        {
            hit_anything = true;
            closet_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}