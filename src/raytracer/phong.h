#pragma once

#include "../lib/vector.h"
#include "../raytracer/trace.h"
#include "../scene/light.h"
#include "../geometry/hittable.h"
#include "../geometry/material.h"

Vector phongIllumination(
    const RT::Trace& trace,
    const SceneLights& lights,
    const std::vector<std::shared_ptr<Hittable>>& objects
);
