#pragma once

#include "../lib/ray.h"
#include "../raytracer/trace.h"
#include "../lib/vector.h" 

class Hittable {
public:
    Vector color;

    Hittable(const Vector& color) : color(color) {}

    virtual RT::Trace hit(const Ray& ray) const = 0;
};
