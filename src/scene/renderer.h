#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <memory>
#include <string>
#include "geometry/hittable.h"
#include "lib/ray.h"
#include "lib/vector.h"
#include "scene/camera.h"

template <typename T>
T clamp(T value, T min, T max) {
    return (value < min) ? min : (value > max) ? max : value;
}

extern std::vector<std::shared_ptr<Hittable>> scene;

Vector color(const Ray& ray);

void render_scene(const Camera& camera, const std::string& filename, uint32_t image_width, uint32_t image_height);

#endif
