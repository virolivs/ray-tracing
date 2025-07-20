#pragma once
#include "../lib/vector.h"

struct Material {
    Vector ka;
    Vector kd;
    Vector ks;
    Vector ke;
    double shininess;
    double ior;
    double opacity;

    Material(
        const Vector& ka = Vector(0, 0, 0),
        const Vector& kd = Vector(1, 1, 1),
        const Vector& ks = Vector(0, 0, 0),
        const Vector& ke = Vector(0, 0, 0),
        double shininess = 1.0,
        double ior = 1.0,
        double opacity = 1.0
    ) : ka(ka), kd(kd), ks(ks), ke(ke),
        shininess(shininess), ior(ior), opacity(opacity) {}
};
