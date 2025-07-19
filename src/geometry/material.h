#pragma once
#include "../lib/vector.h"

struct Material {
    Vector kd;  // difuso
    Vector ks;  // especular
    Vector ka;  // ambiente
    Vector kr;  // reflexão
    Vector kt;  // transmissão
    double eta; // rugosidade

    Material(
        const Vector& kd = Vector(1, 1, 1),
        const Vector& ks = Vector(0, 0, 0),
        const Vector& ka = Vector(0, 0, 0),
        const Vector& kr = Vector(0, 0, 0),
        const Vector& kt = Vector(0, 0, 0),
        double eta = 1.0
    ) : kd(kd), ks(ks), ka(ka), kr(kr), kt(kt), eta(eta) {}
};