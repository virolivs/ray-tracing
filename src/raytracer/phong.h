#pragma once
#include <memory>
#include "../lib/vector.h"
#include "../raytracer/trace.h"
#include "../scene/light.h"
#include "../geometry/hittable.h"
#include "../geometry/material.h"

Vector phongIllumination(
    const RT::Trace& trace,
    const Ray& ray,
    const SceneLights& lights,
    const std::vector<std::shared_ptr<Hittable>>& objects,
    const Material& material
);
