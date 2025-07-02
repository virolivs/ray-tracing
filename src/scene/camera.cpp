#include <cassert>
#include <cmath>
#include "camera.h"

Camera::Camera(Point center, Point target, Vector up, float vertical_fov,
               uint32_t pixel_height, uint32_t pixel_width) : center { center },
                                                              target { target },
                                                              up { up },
                                                              vertical_fov { vertical_fov },
                                                              pixel_height { pixel_height },
                                                              pixel_width { pixel_width }
{
    assert(pixel_height != 0 && pixel_width != 0);
    aspect_ratio = static_cast<float>(pixel_width) / static_cast<float>(pixel_height);

    sensor_height = 2.0f * std::tan(vertical_fov / 2.0f);
    sensor_width = aspect_ratio * sensor_height;

    w = target - center;
    w = w.normalized();
    w = -w;

    v = up - (dot(w, up) / dot(w, w)) * w;
    v = v.normalized();

    u = cross(v, w);
    lower_left_pixel = center - (sensor_width / 2.0f) * u - (sensor_height / 2.0f) * v - w;
}

Ray Camera::cast_ray(const uint32_t& px, const uint32_t& py) const
{
    float sx = (px * sensor_width) / (pixel_width - 1);
    float sy = (py * sensor_height) / (pixel_height - 1);

    Point pixel = lower_left_pixel + sx * u + sy * v;
    Vector direction = (pixel - center).normalized();

    return Ray { center, direction };
}
