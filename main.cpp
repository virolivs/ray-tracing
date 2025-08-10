#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

// Project includes
#include "src/geometry/geometry.h"
#include "src/lib/ray.h"
#include "src/lib/point.h"
#include "src/lib/vector.h"
#include "src/scene/camera.h"
#include "src/scene/renderer.h"
#include "src/scene/light.h"
#include "src/utils/ObjReader.cpp"
#include "src/matrix/matrix.h"
#include "src/matrix/matrixTransforms.h"
#include "src/geometry/bezier.h"

int main() {
    const uint32_t image_width = 500;
    const uint32_t image_height = 500;
    const double vertical_fov = 60.0 * M_PI / 180.0;

    Vector up_vector{0.0, 1.0, 0.0};
    Point look_at{0.0, 0.0, 0.0};
    
// Exemplo 1
    {
        Point camera_position{4.0, 4.0, 6.0};
        Camera camera{camera_position, look_at, up_vector, vertical_fov, image_height, image_width};

        SceneLights lights;
        lights.ambient_color = Vector(0.2f, 0.2f, 0.2f);
        lights.lights.push_back(Light(Point(5.0, 5.0, 5.0), Vector(1.0f, 1.0f, 1.0f)));

        std::vector<std::vector<Point>> control_points(4, std::vector<Point>(4));
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                double x = (i - 1.5);
                double y = (j - 1.5);
                double z = std::cos((x * x + y * y) * 0.7);
                control_points[i][j] = Point(x, z, y); // altura no eixo Y (mantém seu mapeamento)
            }
        }

        Material bezier_material(
            Vector(1.0f, 0.0f, 0.0f),   // ka
            Vector(1.0f, 0.0f, 0.0f),   // kd
            Vector(0.6f, 0.6f, 0.6f),   // ks
            Vector(0.0f),               // ke
            32.0f, 1.0f, 1.0f
        );

        auto bezier_surface = std::make_shared<Geometry::BezierSurface>(control_points, 3, 3, bezier_material);
        scene.push_back(bezier_surface);
        render_scene(camera, "outputs/bezier_dome.ppm", image_width, image_height, lights);
        scene.clear();
    }

    // Exemplo 2
    {
        Point camera_position{4.0, 4.0, 6.0};
        Camera camera{camera_position, look_at, up_vector, vertical_fov, image_height, image_width};

        SceneLights lights;
        lights.ambient_color = Vector(0.2f, 0.2f, 0.2f);
        lights.lights.push_back(Light(Point(5.0, 5.0, 5.0), Vector(1.0f, 1.0f, 1.0f)));

        std::vector<std::vector<Point>> control_points(4, std::vector<Point>(4));
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                double x = (i - 1.5);
                double y = (j - 1.5);
                double z = 0.5 * (x*x - y*y);
                control_points[i][j] = Point(x, z, y);
            }
        }

        Material bezier_material(
            Vector(0.0f, 0.8f, 0.0f),
            Vector(0.0f, 1.0f, 0.0f),
            Vector(0.6f, 0.6f, 0.6f),
            Vector(0.0f),
            32.0f, 1.0f, 1.0f
        );

        auto bezier_surface = std::make_shared<Geometry::BezierSurface>(control_points, 3, 3, bezier_material);
        scene.push_back(bezier_surface);
        render_scene(camera, "outputs/bezier_saddle.ppm", image_width, image_height, lights);
        scene.clear();
    }

    // Exemplo 3
    {
        Point camera_position{4.0, 4.0, 6.0};
        Camera camera{camera_position, look_at, up_vector, vertical_fov, image_height, image_width};

        SceneLights lights;
        lights.ambient_color = Vector(0.2f, 0.2f, 0.2f);
        lights.lights.push_back(Light(Point(5.0, 5.0, 5.0), Vector(1.0f, 1.0f, 1.0f)));

        std::vector<std::vector<Point>> control_points(4, std::vector<Point>(4));
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                double x = (i - 1.5);
                double y = (j - 1.5);
                double z = 0.4 * (x*x + y*y);
                control_points[i][j] = Point(x, z, y);
            }
        }

        Material bezier_material(
            Vector(0.0f, 0.0f, 0.8f),
            Vector(0.0f, 0.0f, 1.0f),
            Vector(0.6f, 0.6f, 0.6f),
            Vector(0.0f),
            32.0f, 1.0f, 1.0f
        );

        auto bezier_surface = std::make_shared<Geometry::BezierSurface>(control_points, 3, 3, bezier_material);
        scene.push_back(bezier_surface);
        render_scene(camera, "outputs/bezier_bowl.ppm", image_width, image_height, lights);
        scene.clear();
    }

    // Exemplo 4
    {
        Point camera_position{4.0, 4.0, 6.0};
        Camera camera{camera_position, look_at, up_vector, vertical_fov, image_height, image_width};

        SceneLights lights;
        lights.ambient_color = Vector(0.2f, 0.2f, 0.2f);
        lights.lights.push_back(Light(Point(5.0, 5.0, 5.0), Vector(1.0f, 1.0f, 1.0f)));

        std::vector<std::vector<Point>> control_points(4, std::vector<Point>(4));
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                double x = (i - 1.5);
                double y = (j - 1.5);
                double z = 0.6 * std::sin(1.2 * x) * std::cos(1.2 * y);
                control_points[i][j] = Point(x, z, y);
            }
        }

        Material bezier_material(
            Vector(0.9f, 0.6f, 0.0f),
            Vector(1.0f, 0.7f, 0.0f),
            Vector(0.6f, 0.6f, 0.6f),
            Vector(0.0f),
            32.0f, 1.0f, 1.0f
        );

        auto bezier_surface = std::make_shared<Geometry::BezierSurface>(control_points, 3, 3, bezier_material);
        scene.push_back(bezier_surface);
        render_scene(camera, "outputs/bezier_wave.ppm", image_width, image_height, lights);
        scene.clear();
    }

    // Exemplo 5
    {
        Point camera_position{4.0, 4.0, 6.0};
        Camera camera{camera_position, look_at, up_vector, vertical_fov, image_height, image_width};

        SceneLights lights;
        lights.ambient_color = Vector(0.2f, 0.2f, 0.2f);
        lights.lights.push_back(Light(Point(5.0, 5.0, 5.0), Vector(1.0f, 1.0f, 1.0f)));

        std::vector<std::vector<Point>> control_points(4, std::vector<Point>(4));
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                double x = (i - 1.5);
                double y = (j - 1.5);
                double r2 = x*x + y*y;
                double z = std::exp(-0.9 * r2);
                control_points[i][j] = Point(x, z, y);
            }
        }

        Material bezier_material(
            Vector(0.6f, 0.0f, 0.8f),
            Vector(0.7f, 0.0f, 1.0f),
            Vector(0.6f, 0.6f, 0.6f),
            Vector(0.0f),
            32.0f, 1.0f, 1.0f
        );

        auto bezier_surface = std::make_shared<Geometry::BezierSurface>(control_points, 3, 3, bezier_material);
        scene.push_back(bezier_surface);
        render_scene(camera, "outputs/bezier_gauss.ppm", image_width, image_height, lights);
        scene.clear();
    }
}