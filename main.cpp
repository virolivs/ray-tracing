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

    // -------------------------------
    // Parte 1: Renderiza o .OBJ
    // -------------------------------

    {
        Point camera_position{3.0, 3.0, 5.0};
        Camera camera{camera_position, look_at, up_vector, vertical_fov, image_height, image_width};

        SceneLights lights;
        lights.ambient_color = Vector(0.1f, 0.1f, 0.1f);
        lights.lights.push_back(Light(Point(5.0, 5.0, 5.0), Vector(1.0, 1.0, 1.0)));

        objReader obj("inputs/cubo.obj");

        const auto& faces = obj.getFaces();
        const auto& vertices = obj.getVertices();

        for (const auto& face : faces) {
            Point p1 = vertices[face.verticeIndice[0]];
            Point p2 = vertices[face.verticeIndice[1]];
            Point p3 = vertices[face.verticeIndice[2]];

            Material m(face.ka, face.kd, face.ks, face.ke, face.ns, face.ni, face.d);
            auto triangle = std::make_shared<Geometry::Triangle>(p1, p2, p3, m);
            scene.push_back(triangle);
        }

        render_scene(camera, "outputs/obj_output.ppm", image_width, image_height, lights);
        scene.clear();
    }

    // -------------------------------
    // Parte 2: Renderiza Bézier
    // -------------------------------

    {
        Point camera_position{4.0, 4.0, 6.0};
        Camera camera{camera_position, look_at, up_vector, vertical_fov, image_height, image_width};

        SceneLights lights;
        lights.ambient_color = Vector(0.2f, 0.2f, 0.2f);
        lights.lights.push_back(Light(Point(5.0, 5.0, 5.0), Vector(1.0f, 1.0f, 1.0f)));

        // Gera pontos de controle (cúpula)
        std::vector<std::vector<Point>> control_points(4, std::vector<Point>(4));
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                double x = (i - 1.5);
                double y = (j - 1.5);
                double z = std::cos((x * x + y * y) * 0.7);
                control_points[i][j] = Point(x, z, y);
            }
        }

        Material bezier_material(
            Vector(0.1f, 0.1f, 0.1f),
            Vector(0.3f, 0.8f, 0.3f),
            Vector(0.5f),
            Vector(0.0f),
            10.0f, 1.0f, 1.0f
        );

        auto bezier_surface = std::make_shared<Geometry::BezierSurface>(
            control_points, 3, 3, bezier_material
        );

        scene.push_back(bezier_surface);

        render_scene(camera, "outputs/bezier_output.ppm", image_width, image_height, lights);
        scene.clear();
    }

    return 0;
}