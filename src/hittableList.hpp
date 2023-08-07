#pragma once

#include "hittable.hpp"
#include "rtweekend.hpp"

#include <memory>
#include <vector>

class HittableList : public Hittable {
public:
    HittableList() { }
    HittableList(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<Hittable> object) { objects.push_back(object); }

    virtual bool hit(
        const Ray& ray, double tMin, double tMax, HitRecord& rec) const override;

public:
    std::vector<shared_ptr<Hittable>> objects;
};

bool HittableList::hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const
{
    HitRecord tempRec;
    bool anyHit  = false;
    auto closest = tMax;

    for (const auto& object : objects) {
        if (object->hit(ray, tMin, closest, tempRec)) {
            anyHit  = true;
            closest = tempRec.t;
            rec     = tempRec;
            // std::cerr << "ray from " << ray.origin << " hit " << rec.p << std::endl;
        }
    }

    return anyHit;
}
