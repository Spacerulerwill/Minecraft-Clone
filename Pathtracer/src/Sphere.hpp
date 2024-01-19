#ifndef SPHERE_H
#define SPHERE_H

#include <Hittable.hpp>
#include <Vector.hpp>

struct Sphere : public Hittable {
    dVec3 center;
    double radius;

    Sphere(dVec3 center, double radius) : center(center), radius(radius) {}

    bool hit(const Ray& r, double ray_tmin, double ray_tmax, HitRecord& rec) const override {
        dVec3 oc = r.origin() - center;
        auto ray_dir_length = r.direction().length();
        auto oc_length = oc.length();
        auto a = ray_dir_length * ray_dir_length;
        auto half_b = oc.dot(r.direction());
        auto c = oc.length() * oc.length() - radius * radius;
        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (root <= ray_tmin || ray_tmax <= root) {
            root = (-half_b + sqrtd) / a;
            if (root <= ray_tmin || ray_tmax <= root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        dVec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }
};

#endif