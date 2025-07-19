#pragma once

#include <vector>
#include "../lib/point.h"
#include "../lib/vector.h"

struct Light {
    Point position {};
    Vector intensity {};

    Light(const Point& pos, const Vector& intensity)
        : position(pos), intensity(intensity) {}
};

struct SceneLights {
    std::vector<Light> lights;
    Vector ambient_color {};

    SceneLights(const Vector& ambient = Vector(0, 0, 0))
        : ambient_color(ambient) {}
};
