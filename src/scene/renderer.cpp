#include "renderer.h"
#include <fstream>
#include <iostream>
#include <limits>

std::vector<std::shared_ptr<Hittable>> scene;

Vector color(const Ray& ray) {
    double closest_t = std::numeric_limits<double>::max();
    Vector final_color;
    bool any_hit = false;

    for (const auto& object : scene) {
        auto hit_result = object->hit(ray);
        if (hit_result.hit && hit_result.t < closest_t) {
            closest_t = hit_result.t;
            final_color = hit_result.color;
            any_hit = true;
        }
    }

    if (!any_hit) {
        Vector unit_direction = ray.direction.normalized();
        double t = 0.5f * (unit_direction.y + 1.0f);
        return Vector(1.0f, 1.0f, 1.0f) * (1.0f - t) + Vector(0.5f, 0.7f, 1.0f) * t;
    }

    return final_color;
}

void render_scene(const Camera& camera, const std::string& filename, uint32_t image_width, uint32_t image_height)
{
    std::ofstream image(filename);
    if (!image)
    {
        std::cerr << "Error creating " << filename << "\n";
        return;
    }

    image << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j)
    {
        for (int i = 0; i < image_width; ++i)
        {
            Vector pixel_color = color(camera.cast_ray(i, j));

            int red   = static_cast<int>(255.99 * clamp(pixel_color.x, 0.0, 1.0));
            int green = static_cast<int>(255.99 * clamp(pixel_color.y, 0.0, 1.0));
            int blue  = static_cast<int>(255.99 * clamp(pixel_color.z, 0.0, 1.0));

            image << red << " " << green << " " << blue << "\n";
        }
    }

    image.close();
    std::cout << "Image saved to " << filename << "\n";
}
