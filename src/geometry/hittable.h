#pragma once

#include "../lib/ray.h"
#include "../raytracer/trace.h"
#include "../lib/vector.h" 

class Hittable {
public:
    Vector color;

    // Default constructor with default color
    Hittable() : color(1.0f, 1.0f, 1.0f) {}

    Hittable(const Vector& color) : color(color) {}

    // Destructor
    ~Hittable() = default;

    virtual RT::Trace hit(const Ray& ray) const = 0;
};
