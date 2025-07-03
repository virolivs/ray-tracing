#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <limits>
#define _USE_MATH_DEFINES
#include <cmath>

#include "src/geometry/geometry.h"
#include "src/geometry/hittable.h"
#include "src/lib/ray.h"
#include "src/lib/point.h"
#include "src/lib/vector.h"
#include "src/scene/camera.h"
#include "src/utils/ObjReader.cpp"


template <typename T>
T clamp(T value, T min, T max) {
    return (value < min) ? min : (value > max) ? max : value;
}

std::vector<std::shared_ptr<Hittable>> scene;

Vector color(const Ray& ray) {
    float closest_t = std::numeric_limits<float>::max();
    Vector final_color;
    bool any_hit = false;

    for (const auto& object : scene) {
        auto hit_result = object->hit(ray);
        if (hit_result.hit && hit_result.t < closest_t) {
            closest_t = hit_result.t;
            final_color = object->color;  
            any_hit = true;
        }
    }

    if (!any_hit) {
        Vector unit_direction = ray.direction.normalized();
        float t = 0.5f * (unit_direction.y + 1.0f);
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

            int red   = static_cast<int>(255.99f * clamp(pixel_color.x, 0.0f, 1.0f));
            int green = static_cast<int>(255.99f * clamp(pixel_color.y, 0.0f, 1.0f));
            int blue  = static_cast<int>(255.99f * clamp(pixel_color.z, 0.0f, 1.0f));

            image << red << " " << green << " " << blue << "\n";
        }
    }

    image.close();
    std::cout << "Image saved to " << filename << "\n";
}

int main() {
    // Test
    // Load OBJ
    objReader obj("inputs/cubo.obj");
    Vector mesh_color(0.8f, 0.3f, 0.3f);  // color example
    Geometry::Mesh mesh(obj, mesh_color);

    Point ray_origin(0.0f, 0.0f, 5.0f);
    Vector ray_direction(0.0f, 0.0f, -1.0f);  
    Ray ray(ray_origin, ray_direction.normalized());

    // Testing intersection //
    RT::Trace trace = mesh.hit(ray);

    if (trace.hit) {
        std::cout << "Hit detected!\n";
        std::cout << "t (distance): " << trace.t << "\n";
        std::cout << "Intersection point: (" << trace.position.x << ", "
                                            << trace.position.y << ", "
                                            << trace.position.z << ")\n";
        std::cout << "Surface normal: (" << trace.normal.x << ", "
                                         << trace.normal.y << ", "
                                         << trace.normal.z << ")\n";
        std::cout << "Mesh color: (" << mesh.color.x << ", "
                                     << mesh.color.y << ", "
                                     << mesh.color.z << ")\n";
    } else {
        std::cout << "No hit detected.\n";
    }

    return 0;
}
