#pragma once
#include "../lib/vector.h"

struct Material {
    Vector ka;        // ambiente
    Vector kd;        // difuso
    Vector ks;        // especular
    Vector ke;        // emissivo
    double shininess; // Ns (brilho)
    double ior;       // Ni (indice de refração)
    double opacity;   // d  (opacidade)

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
