#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <limits>
#include <cmath>

// Project includes
#include "src/geometry/geometry.h"
#include "src/geometry/revolved_bezier.h"
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
    const uint32_t image_width = 500, image_height = 500;
    Point  camera_position(3.0, 3.0, 5.0);
    Point  look_at(0.0, 0.0, 0.0);
    Vector up(0.0, 1.0, 0.0);
    double fov = 60.0 * M_PI / 180.0;
    Camera camera(camera_position, look_at, up, fov, image_height, image_width);

    SceneLights scene_lights;
    scene_lights.lights = {
        Light(Point(10, 10, 10), Vector(1, 1, 1)),
        Light(Point(-5, 5, 5),   Vector(0.5, 0.5, 0.5))
    };

    // Example 1 
    {
        std::vector<std::shared_ptr<Hittable>> scene;
        std::vector<Point> curve = {
            Point(1.0, 0.0, 0.0),
            Point(0.8, 1.0, 0.0),
            Point(0.5, 2.0, 0.0),
            Point(0.0, 3.0, 0.0)
        };
        Material mat;
        mat.kd = Vector(0.8, 0.6, 0.3);
        mat.ka = Vector(0.1, 0.1, 0.1);
        mat.ks = Vector(0.5, 0.5, 0.5);
        mat.shininess = 64.0;

        auto surface = std::make_shared<Geometry::RevolvedBezierSurface>(curve, 3, mat);
        surface->set_resolution(40, 50);
        scene.push_back(surface);

        ::scene = scene;
        render_scene(camera, "outputs/figure1.ppm", image_width, image_height, scene_lights);
    }

    // Example 2
    {
        std::vector<std::shared_ptr<Hittable>> scene;
        std::vector<Point> curve = {
            Point(0.0, 0.0, 0.0),
            Point(0.5, 0.5, 0.0),
            Point(1.5, 1.0, 0.0),
            Point(0.6, 2.0, 0.0)
        };
        Material mat;
        mat.kd = Vector(0.3, 0.6, 0.8);
        mat.ka = Vector(0.1, 0.1, 0.1);
        mat.ks = Vector(0.5, 0.5, 0.5);
        mat.shininess = 64.0;

        auto surface = std::make_shared<Geometry::RevolvedBezierSurface>(curve, 3, mat);
        surface->set_resolution(32, 48);
        scene.push_back(surface);

        ::scene = scene;
        render_scene(camera, "outputs/figure2.ppm", image_width, image_height, scene_lights);
    }

    // Example 3 
    {
        std::vector<std::shared_ptr<Hittable>> scene;
        std::vector<Point> curve = {
            Point(0.4, 0.0, 0.0),
            Point(0.6, 0.8, 0.0),
            Point(1.2, 1.5, 0.0),
            Point(0.5, 2.5, 0.0)
        };
        Material mat;
        mat.kd = Vector(0.6, 0.8, 0.3);
        mat.ka = Vector(0.1, 0.1, 0.1);
        mat.ks = Vector(0.5, 0.5, 0.5);
        mat.shininess = 64.0;

        auto surface = std::make_shared<Geometry::RevolvedBezierSurface>(curve, 3, mat);
        surface->set_resolution(24, 32);
        scene.push_back(surface);

        ::scene = scene;
        render_scene(camera, "outputs/figure3.ppm", image_width, image_height, scene_lights);
    }

    // Example 4
    {
        std::vector<std::shared_ptr<Hittable>> scene;
        std::vector<Point> curve = {
            Point(1.0, 0.0, 0.0),
            Point(1.0, 1.0, 0.0),
            Point(1.0, 2.0, 0.0),
            Point(1.0, 3.0, 0.0)
        };
        Material mat;
        mat.kd = Vector(0.8, 0.3, 0.6);
        mat.ka = Vector(0.1, 0.1, 0.1);
        mat.ks = Vector(0.5, 0.5, 0.5);
        mat.shininess = 64.0;

        auto surface = std::make_shared<Geometry::RevolvedBezierSurface>(curve, 3, mat);
        surface->set_resolution(20, 28);
        scene.push_back(surface);

        ::scene = scene;
        render_scene(camera, "outputs/figure4.ppm", image_width, image_height, scene_lights);
    }

    // Example 5
    {
        std::vector<std::shared_ptr<Hittable>> scene;
        std::vector<Point> curve = {
            Point(0.0, 0.0, 0.0),
            Point(0.8, 0.5, 0.0),
            Point(1.5, 1.5, 0.0),
            Point(0.0, 2.0, 0.0)
        };
        Material mat;
        mat.kd = Vector(0.9, 0.9, 0.2);
        mat.ka = Vector(0.1, 0.1, 0.1);
        mat.ks = Vector(0.5, 0.5, 0.5);
        mat.shininess = 64.0;

        auto surface = std::make_shared<Geometry::RevolvedBezierSurface>(curve, 3, mat);
        surface->set_resolution(20, 30);
        scene.push_back(surface);

        ::scene = scene;
        render_scene(camera, "outputs/figure5.ppm", image_width, image_height, scene_lights);
    }


    return 0;
}