#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <limits>
#include <cmath>

#include "src/geometry/geometry.h"
#include "src/geometry/hittable.h"
#include "src/lib/ray.h"
#include "src/lib/point.h"
#include "src/lib/vector.h"
#include "src/scene/camera.h"
#include "src/utils/ObjReader.cpp"
#include "src/scene/renderer.h"
#include "src/matrix/matrix.h"
#include "src/matrix/matrixTransforms.h"
#include "src/scene/light.h"  

int main() {
    // Image and camera params
    Point camera_position { 3.0, 3.0, 5.0 };
    Point look_at { 0.0, 0.0, 0.0 };
    Vector up_vector { 0.0, 1.0, 0.0 };
    double vertical_fov = 60.0 * M_PI / 180.0;
    uint32_t image_height = 500;
    uint32_t image_width = 500;

    // Camera initialization
    Camera camera { camera_position, look_at, up_vector, vertical_fov, image_height, image_width };

    // Object reading
    objReader obj("inputs/cubo.obj");

    // Scene lights
    SceneLights lights;
    lights.ambient_color = Vector(0.2f, 0.2f, 0.2f);
    lights.lights.push_back(Light(Point(5.0, 5.0, 5.0), Vector(1.0, 1.0, 1.0)));
    lights.lights.push_back(Light(Point(1.0, 2.0, 3.0), Vector(1.0, 1.0, 1.0)));


    ///////////////////////////
    // FIVE SPHERES + PLANE
    ///////////////////////////

    lights.lights.pop_back();
    Point camera_position_spheres { 0.0, 1.5f, 5.0 };
    Camera camera_spheres { camera_position_spheres, look_at, up_vector, vertical_fov, image_height, image_width };
    lights.lights.push_back(Light(Point(5.0, 5.0, 5.0), Vector(0.5f, 0.5f, 0.5f)));

    // Esfera com reflexão (espelho), opacidade 1, ior irrelevante
    Material reflective_material(
        Vector(0.0f, 0.0f, 0.0f),  // ka
        Vector(0.0f, 0.0f, 0.0f),  // kd (sem difuso, só espelho)
        Vector(1.0f, 1.0f, 1.0f),  // ks (alta especularidade)
        Vector(0.0f, 0.0f, 0.0f),  // ke
        100.0,                     // shininess (bem brilhante)
        1.0,                       // ior (não usado em reflexão)
        1.0                        // opacity (sólido)
    );

    // Esfera com refração (transparente, vidro)
    Material refractive_material(
        Vector(0.1f, 0.1f, 0.8f),   // ka
        Vector(0.1f, 0.1f, 0.8f),  // kd (sem cor difusa)
        Vector(0.5f, 0.5f, 0.5f),  // ks (leve brilho)
        Vector(0.0f, 0.0f, 1.0f),  // ke (sem emissão)
        10.0,                      // shininess
        1.0,                       // ior (vidro típico)
        0.1                        // opacity (transparente)
    );

    // Material para esfera verde (sem alteração)
    Material green_material(
        Vector(0.0, 0.3f, 0.0),     // ka
        Vector(0.0, 0.7f, 0.0),     // kd
        Vector(0.0f, 0.0f, 0.0f),   // ks
        Vector(0.1f, 0.1f, 0.1f),   // ke
        5.0,                        // shininess
        1.0,                        // ior
        1.0                         // opacity
    );

    // Esfera pequena 1 (azul clara)
    Material blue_material(
        Vector(0.0f, 0.0f, 0.3f),   // ka
        Vector(0.0f, 0.0f, 0.7f),   // kd
        Vector(0.1f, 0.1f, 0.1f),   // ks
        Vector(0.1f, 0.1f, 0.1f),   // ke
        5.0,                        // shininess
        1.0,                        // ior
        1.0                         // opacity
    );

    // Esfera pequena 2 (vermelha clara)
    Material red_material(
        Vector(0.3f, 0.0f, 0.0f),   // ka
        Vector(0.7f, 0.0f, 0.0f),   // kd
        Vector(0.0f, 0.0f, 0.0f),   // ks
        Vector(0.1f, 0.1f, 0.1f),   // ke
        5.0,                        // shininess
        1.0,                        // ior
        1.0                         // opacity
    );

    Material gray_material(
        Vector(0.1f, 0.1f, 0.1f), // ka 
        Vector(0.5f, 0.5f, 0.5f), // kd 
        Vector(0.2f, 0.2f, 0.2f), // ks
        Vector(0.0, 0.0, 0.0),    // ke 
        1.0,                      // shininess (não relevante sem ks)
        1.0,                      // ior
        1.0                       // opacity (totalmente opaco)
    );

    auto sphere_reflective = std::make_shared<Geometry::Sphere>(
        Point(-1.0, 0.9f, 0.0),
        1.0,
        reflective_material
    );

    auto sphere_refractive = std::make_shared<Geometry::Sphere>(
        Point(1.0, 0.5f, 1.5f),
        1.0,
        refractive_material
    );

    auto sphere_green = std::make_shared<Geometry::Sphere>(
        Point(-1.0, 0.5f, 2.5f),
        0.5f,
        green_material
    );

    auto small_sphere1 = std::make_shared<Geometry::Sphere>(
        Point(0.0f, 0.2f, 3.5f),  // próxima da esfera verde
        0.25f,
        blue_material
    );

    auto small_sphere2 = std::make_shared<Geometry::Sphere>(
        Point(1.0f, 0.1f, 3.0f),  // próxima da esfera verde
        0.1f,
        red_material
    );

    auto plane = std::make_shared<Geometry::Plane>(
        Point(0.0, 0.0, 0.0),
        Vector(0.0, 1.0, 0.0),
        gray_material
    );

    scene.push_back(sphere_reflective);
    scene.push_back(sphere_refractive);
    scene.push_back(sphere_green);
    scene.push_back(small_sphere1);
    scene.push_back(small_sphere2);
    scene.push_back(plane);

    render_scene(camera_spheres, "outputs/five_spheres_plane.ppm", image_width, image_height, lights);
    scene.clear();

    return 0;
}