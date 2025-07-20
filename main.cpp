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
    // CUBE TEST CASES
    ///////////////////////////

    // Original form
    auto original_mesh = std::make_shared<Geometry::Mesh>(obj);
    scene.push_back(original_mesh);
    render_scene(camera, "outputs/original.ppm", image_width, image_height, lights);
    scene.clear();

    // Translation transformation
    Matrix translation = translationMatrix(0.0, 0.0, 2.0);
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
    Matrix scale = scaleMatrix(1.0, 2.0, 0.5f);  
    auto scaled_mesh = Geometry::transformMesh(*original_mesh, scale);
    scene.push_back(scaled_mesh);
    render_scene(camera, "outputs/scaled.ppm", image_width, image_height, lights);
    scene.clear();

    // Shearing transformation
    Matrix shear = shearMatrix(0.5f, 0.0, 0.0, 0.0, 0.0, 0.0);
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
        translationMatrix(0.0, -1.0, -1.0) *
        rotationMatrix('Y', M_PI / 6) *
        rotationMatrix('X', M_PI / 6) *
        shearMatrix(0.3f, 0.0, 0.0, 0.0, 0.0, 0.0) *
        scaleMatrix(1.2f, 0.8f, 1.0);

    auto combined_mesh = Geometry::transformMesh(*original_mesh, combined);
    scene.push_back(combined_mesh);
    render_scene(camera, "outputs/combined.ppm", image_width, image_height, lights);
    scene.clear();



    ///////////////////////////
    // TWO SPHERES + PLANE
    ///////////////////////////
    
    lights.lights.pop_back();
    Point camera_position_spheres { 0.0, 1.5f, 5.0 };
    Camera camera_spheres { camera_position_spheres, look_at, up_vector, vertical_fov, image_height, image_width };
    lights.lights.push_back(Light(Point(5.0, 5.0, 5.0), Vector(0.5f, 0.5f, 0.5f)));


    // Material for sphere 1 (red)
    Material red_material(
        Vector(0.1f, 0.0, 0.0), // ka 
        Vector(0.7f, 0.0, 0.0), // kd    
        Vector(0.5f, 0.5f, 0.5f), // ks 
        Vector(0.0, 0.0, 0.0), // ke  
        50.0,                    // shininess 
        1.0,                     // ior       
        1.0                      // opacity    
    );

    // Material for sphere 2 (blue)
    Material blue_material(
        Vector(0.0, 0.0, 0.1f), // ka 
        Vector(0.0, 0.0, 0.7f), // kd 
        Vector(1.0, 1.0, 1.0), // ks 
        Vector(0.0, 0.0, 0.0), // ke 
        50.0,                    // shininess 
        1.0,                     // ior 
        1.0                      // opacity 
    );

    // Material for sphere 3 (green)
    Material green_material(
        Vector(0.0, 0.1f, 0.0), // ka 
        Vector(0.0, 0.7f, 0.0), // kd 
        Vector(0.5f, 0.5f, 0.5f), // ks 
        Vector(0.0, 0.0, 0.0), // ke 
        50.0,                    // shininess 
        1.0,                     // ior 
        1.0                      // opacity 
    );

    // Material for plane (grey)
    Material gray_material(
        Vector(0.1f, 0.1f, 0.1f), // ka 
        Vector(0.5f, 0.5f, 0.5f), // kd 
        Vector(0.2f, 0.2f, 0.2f), // ks 
        Vector(0.0, 0.0, 0.0), // ke 
        10.0,                    // shininess 
        1.0,                     // ior 
        1.0                      // opacity 
    );

    auto sphere1 = std::make_shared<Geometry::Sphere>(
        Point(-1.0, 1.5f, 0.0),
        1.0,
        red_material
    );

    auto sphere2 = std::make_shared<Geometry::Sphere>(
        Point(1.0, 0.5f, 1.5f),
        1.0,
        blue_material
    );

     auto sphere3 = std::make_shared<Geometry::Sphere>(
        Point(0.0, 0.5f, 0.5f), 
        0.5f,                    
        green_material
    );

    auto plane = std::make_shared<Geometry::Plane>(
        Point(0.0, 0.0, 0.0),
        Vector(0.0, 1.0, 0.0),
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
