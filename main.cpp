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
#include "src/matrix/matrix.h"
#include "src/matrix/matrixTransforms.h"

template <typename T>
T clamp(T value, T min, T max) {
    return (value < min) ? min : (value > max) ? max : value;
}

std::vector<std::shared_ptr<Hittable>> scene;

Vector color(const Ray& ray) {
    // Initialize the closest intersection distance to a very large number
    float closest_t = std::numeric_limits<float>::max();

    // Store the resulting color and a flag to check if the ray hits anything
    Vector final_color;
    bool any_hit = false;

    // Iterate over all objects in the scene and find the closest intersection
    for (const auto& object : scene) {
        auto hit_result = object->hit(ray);
        if (hit_result.hit && hit_result.t < closest_t) {
            closest_t = hit_result.t;
            final_color = object->color;  // Use the object's color if it's the closest hit so far
            any_hit = true;
        }
    }

    // If the ray didn't hit anything, return a gradient background color (sky)
    if (!any_hit) {
        Vector unit_direction = ray.direction.normalized();
        float t = 0.5f * (unit_direction.y + 1.0f);

        // Interpolates between white and blue depending on the ray's vertical direction
        return Vector(1.0f, 1.0f, 1.0f) * (1.0f - t) + Vector(0.5f, 0.7f, 1.0f) * t;
    }

    // Return the color of the closest object hit by the ray
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
    // Camera setup
    Point camera_position { 3.0f, 3.0f, 5.0f };  // Camera positioned off the main axes for better 3D perception
    Point look_at { 0.0f, 0.0f, 0.0f };
    Vector up_vector { 0.0f, 1.0f, 0.0f };
    float vertical_fov = 90.0f * M_PI / 180.0f;
    uint32_t image_height = 500;
    uint32_t image_width = 500;

    // Load mesh from OBJ file
    objReader obj("inputs/cubo.obj");
    auto original_mesh = std::make_shared<Geometry::Mesh>(obj, Vector(1.0f, 0.0f, 0.0f)); // Red color for original mesh

    // Camera creation
    Camera camera { camera_position, look_at, up_vector, vertical_fov, image_height, image_width };

    scene.push_back(original_mesh);
    render_scene(camera, "outputs/original.ppm", image_width, image_height);

    // TEST 1: Translation along Z-axis (depth effect)
    Matrix translation = translationMatrix(0.0f, 0.0f, -2.0f);  
    auto translated_mesh = Geometry::transformMesh(*original_mesh, translation);
    translated_mesh->color = Vector(0.0f, 0.0f, 1.0f);  // Blue color

    scene.clear();
    scene.push_back(translated_mesh);
    render_scene(camera, "outputs/test1_translation_z.ppm", image_width, image_height);

    // TEST 2: Rotation around Y-axis (lateral rotation)
    Matrix rotation_y = rotationYMatrix(M_PI / 4);  
    auto rotated_y_mesh = Geometry::transformMesh(*original_mesh, rotation_y);
    rotated_y_mesh->color = Vector(0.0f, 1.0f, 0.0f);  // Green color

    scene.clear();
    scene.push_back(rotated_y_mesh);
    render_scene(camera, "outputs/test2_rotation_y.ppm", image_width, image_height);

    // TEST 3: Rotation around X-axis (tilt up/down)
    Matrix rotation_x = rotationXMatrix(M_PI / 6);  
    auto rotated_x_mesh = Geometry::transformMesh(*original_mesh, rotation_x);
    rotated_x_mesh->color = Vector(1.0f, 1.0f, 0.0f);  // Yellow color

    scene.clear();
    scene.push_back(rotated_x_mesh);
    render_scene(camera, "outputs/test3_rotation_x.ppm", image_width, image_height);

    // TEST 4: Non-uniform scaling (deforms the cube)
    Matrix scale = scaleMatrix(1.0f, 2.0f, 0.5f);  
    auto scaled_mesh = Geometry::transformMesh(*original_mesh, scale);
    scaled_mesh->color = Vector(1.0f, 0.0f, 1.0f);  // Magenta color

    scene.clear();
    scene.push_back(scaled_mesh);
    render_scene(camera, "outputs/test4_nonuniform_scale.ppm", image_width, image_height);

    // TEST 5: Combined transformations - translation + rotations on X and Y axes
    Matrix combined = translationMatrix(0.0f, -1.0f, -1.0f) *
                      rotationYMatrix(M_PI / 6) *
                      rotationXMatrix(M_PI / 6);
    auto combined_mesh = Geometry::transformMesh(*original_mesh, combined);
    combined_mesh->color = Vector(0.3f, 0.7f, 0.2f);  // Custom greenish color

    scene.clear();
    scene.push_back(combined_mesh);
    render_scene(camera, "outputs/test5_combined.ppm", image_width, image_height);

    return 0;
}
