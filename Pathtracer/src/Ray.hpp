#ifndef RAY_H
#define RAY_H

#include <Vector.hpp>

class Ray {
public:
    Ray() {}

    Ray(const dVec3& origin, const dVec3& direction) : orig(origin), dir(direction) {}

    dVec3 origin() const { return orig; }
    dVec3 direction() const { return dir; }

    dVec3 at(double t) const {
        return orig + t * dir;
    }

private:
    dVec3 orig;
    dVec3 dir;
};

#endif