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
    Point camera_position { 3.0f, 3.0f, 5.0f };
    Point look_at { 0.0f, 0.0f, 0.0f };
    Vector up_vector { 0.0f, 1.0f, 0.0f };
    double vertical_fov = 60.0f * M_PI / 180.0f;
    uint32_t image_height = 500;
    uint32_t image_width = 500;

    // Camera initialization
    Camera camera { camera_position, look_at, up_vector, vertical_fov, image_height, image_width };

    // Object reading
    objReader obj("inputs/cubo.obj");

    // Scene lights
    SceneLights lights;
    lights.ambient_color = Vector(0.2f, 0.2f, 0.2f);
    lights.lights.push_back(Light(Point(5.0f, 5.0f, 5.0f), Vector(1.0f, 1.0f, 1.0f)));
    lights.lights.push_back(Light(Point(1.0f, 2.0f, 3.0f), Vector(1.0f, 1.0f, 1.0f)));

    ///////////////////////////
    // CUBE TEST CASES
    ///////////////////////////

    // Original form
    auto original_mesh = std::make_shared<Geometry::Mesh>(obj);
    scene.push_back(original_mesh);
    render_scene(camera, "outputs/original.ppm", image_width, image_height, lights);
    scene.clear();

    // Translation transformation
    Matrix translation = translationMatrix(0.0f, 0.0f, 2.0f);
    auto transladed_mesh = Geometry::transformMesh(*original_mesh, translation);
    scene.push_back(transladed_mesh);
    render_scene(camera, "outputs/transladed.ppm", image_width, image_height, lights);
    scene.clear();

    // Rotation around Y axis
    Matrix rotation_y = rotationMatrix('Y', M_PI / 4);
    auto rotated_mesh = Geometry::transformMesh(*original_mesh, rotation_y);
    scene.push_back(rotated_mesh);
    render_scene(camera, "outputs/rotated.ppm", image_width, image_height, lights);
    scene.clear();

    // Scaling transformation
    Matrix scale = scaleMatrix(1.0f, 2.0f, 0.5f);  
    auto scaled_mesh = Geometry::transformMesh(*original_mesh, scale);
    scene.push_back(scaled_mesh);
    render_scene(camera, "outputs/scaled.ppm", image_width, image_height, lights);
    scene.clear();

    // Shearing transformation
    Matrix shear = shearMatrix(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    auto sheared_mesh = Geometry::transformMesh(*original_mesh, shear);
    scene.push_back(sheared_mesh);
    render_scene(camera, "outputs/sheared.ppm", image_width, image_height, lights);
    scene.clear();

    // Reflection over Z axis
    Matrix reflection = reflectionMatrix(false, false, true);
    auto reflected_mesh = Geometry::transformMesh(*original_mesh, reflection);
    scene.push_back(reflected_mesh);
    render_scene(camera, "outputs/reflected.ppm", image_width, image_height, lights);
    scene.clear();

    // Combined transformation: translate, rotate, shear, and scale
    Matrix combined = 
        translationMatrix(0.0f, -1.0f, -1.0f) *
        rotationMatrix('Y', M_PI / 6) *
        rotationMatrix('X', M_PI / 6) *
        shearMatrix(0.3f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) *
        scaleMatrix(1.2f, 0.8f, 1.0f);

    auto combined_mesh = Geometry::transformMesh(*original_mesh, combined);
    scene.push_back(combined_mesh);
    render_scene(camera, "outputs/combined.ppm", image_width, image_height, lights);
    scene.clear();



    ///////////////////////////
    // TWO SPHERES + PLANE
    ///////////////////////////
    
    lights.lights.pop_back();
    Point camera_position_spheres { 0.0f, 1.5f, 5.0f };
    Camera camera_spheres { camera_position_spheres, look_at, up_vector, vertical_fov, image_height, image_width };
    lights.lights.push_back(Light(Point(5.0f, 5.0f, 5.0f), Vector(0.5f, 0.5f, 0.5f)));


    // Material for sphere 1 (red)
    Material red_material(
        Vector(0.1f, 0.0f, 0.0f), // ka 
        Vector(0.7f, 0.0f, 0.0f), // kd    
        Vector(0.5f, 0.5f, 0.5f), // ks 
        Vector(0.0f, 0.0f, 0.0f), // ke  
        50.0f,                    // shininess 
        1.0f,                     // ior       
        1.0f                      // opacity    
    );

    // Material for sphere 2 (blue)
    Material blue_material(
        Vector(0.0f, 0.0f, 0.1f), // ka 
        Vector(0.0f, 0.0f, 0.7f), // kd 
        Vector(1.0f, 1.0f, 1.0f), // ks 
        Vector(0.0f, 0.0f, 0.0f), // ke 
        50.0f,                    // shininess 
        1.0f,                     // ior 
        1.0f                      // opacity 
    );

    // Material for sphere 3 (green)
    Material green_material(
        Vector(0.0f, 0.1f, 0.0f), // ka 
        Vector(0.0f, 0.7f, 0.0f), // kd 
        Vector(0.5f, 0.5f, 0.5f), // ks 
        Vector(0.0f, 0.0f, 0.0f), // ke 
        50.0f,                    // shininess 
        1.0f,                     // ior 
        1.0f                      // opacity 
    );

    // Material for plane (grey)
    Material gray_material(
        Vector(0.1f, 0.1f, 0.1f), // ka 
        Vector(0.5f, 0.5f, 0.5f), // kd 
        Vector(0.2f, 0.2f, 0.2f), // ks 
        Vector(0.0f, 0.0f, 0.0f), // ke 
        10.0f,                    // shininess 
        1.0f,                     // ior 
        1.0f                      // opacity 
    );

    auto sphere1 = std::make_shared<Geometry::Sphere>(
        Point(-1.0f, 1.5f, 0.0f),
        1.0f,
        red_material
    );

    auto sphere2 = std::make_shared<Geometry::Sphere>(
        Point(1.0f, 0.5f, 1.5f),
        1.0f,
        blue_material
    );

     auto sphere3 = std::make_shared<Geometry::Sphere>(
        Point(0.0f, 0.5f, 0.5f), 
        0.5f,                    
        green_material
    );

    auto plane = std::make_shared<Geometry::Plane>(
        Point(0.0f, 0.0f, 0.0f),
        Vector(0.0f, 1.0f, 0.0f),
        gray_material
    );

    scene.push_back(sphere1);
    scene.push_back(sphere2);
    scene.push_back(sphere3);
    scene.push_back(plane);

    render_scene(camera_spheres, "outputs/two_spheres_plane.ppm", image_width, image_height, lights);
    scene.clear();

    return 0;
}
