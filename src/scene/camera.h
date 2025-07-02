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

    float vertical_fov {};
    float aspect_ratio {};

    float sensor_height, sensor_width {};   // world dimensions
    uint32_t pixel_height, pixel_width {};  // pixel dimensions

    Point lower_left_pixel {};

public:
    explicit Camera(Point center, Point target, Vector up, float vertical_fov,
                    uint32_t pixel_height, uint32_t pixel_width);

    uint32_t get_pixel_width() const { return pixel_width; }
    uint32_t get_pixel_height() const { return pixel_height; }

    Camera() = default;
    Camera(const Camera &) = default;
    ~Camera() = default;
    Camera &operator=(const Camera &) = default;

    Ray cast_ray(const uint32_t& px, const uint32_t& py) const;
};
