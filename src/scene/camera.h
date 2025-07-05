#pragma once

#include <cstdint>
#include "../lib/point.h"
#include "../lib/ray.h"
#include "../lib/vector.h"

class Camera
{
private:
    Point center {};
    Point target {};

    Vector up {};
    Vector u, v, w {};

    double vertical_fov {};
    double aspect_ratio {};

    // world dimensions
    double sensor_height, sensor_width {};

    // pixel dimensions
    uint32_t pixel_height, pixel_width {};

    Point lower_left_pixel {};

public:
    explicit Camera(Point center, Point target, Vector up, double vertical_fov, uint32_t pixel_height, uint32_t pixel_width);

    uint32_t get_pixel_width() const { return pixel_width; }
    uint32_t get_pixel_height() const { return pixel_height; }

    Camera() = default;
    Camera(const Camera &) = default;
    ~Camera() = default;
    Camera &operator=(const Camera &) = default;

    Ray cast_ray(const uint32_t& px, const uint32_t& py) const;
};
