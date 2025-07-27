#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <limits>
#include <cmath>

// Project includes
#include "src/geometry/geometry.h"
#include "src/geometry/hittable.h"
#include "src/lib/ray.h"
#include "src/lib/point.h"
#include "src/lib/vector.h"
#include "src/scene/camera.h"
#include "src/scene/renderer.h"
#include "src/scene/light.h"
#include "src/utils/ObjReader.cpp"
#include "src/matrix/matrix.h"
#include "src/matrix/matrixTransforms.h"

int main() {
    // Image & Camera Setup
    const uint32_t image_width = 500;
    const uint32_t image_height = 500;
    const double vertical_fov = 60.0 * M_PI / 180.0;
    Point look_at{0.0, 0.0, 0.0};
    Vector up_vector{0.0, 1.0, 0.0};
    Point camera_position{3.0, 3.0, 5.0};
    Camera camera{camera_position, look_at, up_vector, vertical_fov, image_height, image_width};

    // Load Objects (OBJ)
    objReader obj("inputs/cubo.obj");

    // Lighting Setup
    SceneLights lights;
    lights.ambient_color = Vector(0.2f, 0.2f, 0.2f);
    lights.lights.push_back(Light(Point(5.0, 5.0, 5.0), Vector(1.0, 1.0, 1.0)));
    lights.lights.push_back(Light(Point(1.0, 2.0, 3.0), Vector(1.0, 1.0, 1.0)));

    // Alternate Camera & Light for Three Spheres + Plane Scene
    lights.lights.pop_back();  // Remove one light
    Point alt_camera_position{0.0, 1.5f, 5.0};
    Camera alt_camera{alt_camera_position, look_at, up_vector, vertical_fov, image_height, image_width};
    lights.lights.push_back(Light(Point(5.0, 5.0, 5.0), Vector(0.5f, 0.5f, 0.5f)));

    ///////// Materials

    // Reflective (mirror-like)
    Material reflective_material(
        Vector(0.0f), Vector(0.0f), Vector(1.0f), Vector(0.0f),
        100.0f, 1.5f, 0.1f
    );

    // Refractive (glass-like)
    Material refractive_material(
        Vector(0.1f), Vector(0.7f), Vector(0.1f), Vector(0.0f),
        50.0f, 1.5f, 0.0f
    );

    // Opaque green
    Material green_material(
        Vector(0.0f, 0.3f, 0.0f), Vector(0.0f, 0.7f, 0.0f),
        Vector(0.0f), Vector(0.1f), 5.0f, 1.0f, 1.0f
    );

    // Opaque gray (floor)
    Material gray_material(
        Vector(0.1f), Vector(0.5f), Vector(0.2f), Vector(0.0f),
        1.0f, 1.0f, 1.0f
    );

    // Opaque red
    Material red_material(
        Vector(0.1f, 0.0f, 0.0f), Vector(0.8f, 0.1f, 0.1f),
        Vector(0.0f), Vector(0.0f), 20.0f, 1.0f, 1.0f
    );

    // Opaque blue
    Material blue_material(
        Vector(0.0f, 0.0f, 0.3f), Vector(0.0f, 0.0f, 0.7f),
        Vector(0.0f), Vector(0.0f), 5.0f, 1.0f, 1.0f
    );

    // Scene Geometry
    auto mirror_sphere = std::make_shared<Geometry::Sphere>(
        Point(-1.0, 0.75f, 0.0), 1.0f, reflective_material
    );

    auto glass_sphere = std::make_shared<Geometry::Sphere>(
        Point(0.7f, 0.5f, 2.0f), 0.5f, refractive_material
    );

    auto red_sphere_behind = std::make_shared<Geometry::Sphere>(
        Point(1.0, 0.5f, -1.0f), 0.8f, red_material
    );

    auto green_sphere = std::make_shared<Geometry::Sphere>(
        Point(-1.0, 0.5f, 2.5f), 0.5f, green_material
    );

    auto small_blue_sphere = std::make_shared<Geometry::Sphere>(
        Point(0.0f, 0.2f, 3.0f), 0.2f, blue_material
    );

    auto ground_plane = std::make_shared<Geometry::Plane>(
        Point(0.0, 0.0, 0.0), Vector(0.0, 1.0, 0.0), gray_material
    );

    // Scene Composition
    scene.push_back(mirror_sphere);
    scene.push_back(glass_sphere);
    scene.push_back(red_sphere_behind);
    scene.push_back(green_sphere);
    scene.push_back(small_blue_sphere);
    scene.push_back(ground_plane);

    // Render & Output
    render_scene(alt_camera, "outputs/output.ppm", image_width, image_height, lights);
    scene.clear();

    return 0;
}
