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


int main() {
    // Image and camera params
    Point camera_position { 3.0f, 3.0f, 5.0f };
    Point look_at { 0.0f, 0.0f, 0.0f };
    Vector up_vector { 0.0f, 1.0f, 0.0f };
    double vertical_fov = 90.0f * M_PI / 180.0f; // radians
    uint32_t image_height = 500;
    uint32_t image_width = 500;

    // Initializes the camera with the defined parameters
    Camera camera { camera_position, look_at, up_vector, vertical_fov, image_height, image_width };

    // Load the object
    objReader obj("inputs/cubo.obj");

    ////// OBJECT VISUALIZATION //////

    // TEST 0: Object visualization
    auto original_mesh = std::make_shared<Geometry::Mesh>(obj, obj.getKd());
    scene.push_back(original_mesh);
    render_scene(camera, "outputs/original.ppm", image_width, image_height);
    scene.clear();

    // TEST 1: Translation
    Matrix translation = translationMatrix(0.0f, 0.0f, 2.0f);
    auto transladed_mesh = Geometry::transformMesh(*original_mesh, translation);
    scene.push_back(transladed_mesh);
    render_scene(camera, "outputs/transladed.ppm", image_width, image_height);
    scene.clear();

    // TEST 2: Rotation
    Matrix rotation_y = rotationMatrix('Y', M_PI / 4);
    auto rotated_mesh = Geometry::transformMesh(*original_mesh, rotation_y);
    scene.push_back(rotated_mesh);
    render_scene(camera, "outputs/rotated.ppm", image_width, image_height);
    scene.clear();

    // TEST 3: Scale
    Matrix scale = scaleMatrix(1.0f, 2.0f, 0.5f);  
    auto scaled_mesh = Geometry::transformMesh(*original_mesh, scale);
    scene.push_back(scaled_mesh);
    render_scene(camera, "outputs/scaled.ppm", image_width, image_height);
    scene.clear();

    // TEST 4: Shear matrix
    Matrix shear = shearMatrix(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    auto sheared_mesh = Geometry::transformMesh(*original_mesh, shear);
    scene.push_back(sheared_mesh);
    render_scene(camera, "outputs/sheared.ppm", image_width, image_height);
    scene.clear();

    // TEST 5: Reflection across the XY plane (inverte Z)
    Matrix reflection = reflectionMatrix(false, false, true);
    auto reflected_mesh = Geometry::transformMesh(*original_mesh, reflection);
    scene.push_back(reflected_mesh);
    render_scene(camera, "outputs/reflected.ppm", image_width, image_height);
    scene.clear();

    // TEST 6: Combined matrix
    Matrix combined = 
        translationMatrix(0.0f, -1.0f, -1.0f) * rotationMatrix('Y', M_PI / 6) * rotationMatrix('X', M_PI / 6) * 
        shearMatrix(0.3f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) * scaleMatrix(1.2f, 0.8f, 1.0f);

    auto combined_mesh = Geometry::transformMesh(*original_mesh, combined);
    scene.push_back(combined_mesh);
    render_scene(camera, "outputs/combined.ppm", image_width, image_height);
    scene.clear();

    return 0;
}