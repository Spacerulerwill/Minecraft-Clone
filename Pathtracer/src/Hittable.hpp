#ifndef HITTABLE_H
#define HITTABLE_H

#include <Ray.hpp>

class HitRecord {
public:
    dVec3 p;
    dVec3 normal;
    double t;
    bool front_face;

    void set_face_normal(const Ray& r, const dVec3& outward_normal) {
        front_face = (r.direction().dot(outward_normal)) < 0;
        normal = front_face ? outward_normal : -1.0 * outward_normal;
    }
};

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& r, double ray_tmin, double ray_tmax, HitRecord& rec) const = 0;
};

#endif