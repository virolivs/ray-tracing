#pragma once

#include "../lib/ray.h"
#include "../raytracer/trace.h"
#include "../lib/vector.h"
#include "material.h"

class Hittable {
public:
    Material material;

    Hittable() : material() {}

    Hittable(const Material& material) : material(material) {}

    virtual ~Hittable() = default;

    virtual RT::Trace hit(const Ray& ray) const = 0;

    virtual const Material& get_material(int index = -1) const {
        return material;
    }
    
};