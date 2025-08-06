#include "bezier.h"
#include <cmath>
#include <limits>

namespace Geometry {

// Construtor
BezierSurface::BezierSurface(
    const std::vector<std::vector<Point>>& control_points,
    int degree_u,
    int degree_v,
    const Material& material
) : Hittable(material), control_points(control_points),
    degree_u(degree_u), degree_v(degree_v) {}

// Bernstein polinomial
static double bernstein(int i, int n, double t) {
    auto binomial = [](int n, int k) {
        double res = 1.0;
        for (int j = 1; j <= k; ++j)
            res *= double(n - j + 1) / j;
        return res;
    };
    return binomial(n, i) * pow(t, i) * pow(1 - t, n - i);
}

// Avaliação da superfície Bézier
Point BezierSurface::evaluate(double u, double v) const {
    Point p(0, 0, 0);
    for (int i = 0; i <= degree_u; ++i) {
        for (int j = 0; j <= degree_v; ++j) {
            double bu = bernstein(i, degree_u, u);
            double bv = bernstein(j, degree_v, v);
            Vector contrib = (control_points[i][j] - Point(0, 0, 0)) * (bu * bv);
            p += contrib;
        }
    }
    return p;
}

// Gera os triângulos da tesselação (20x20 por padrão)
void BezierSurface::generateTriangles(std::vector<Triangle>& outTriangles, int resolution) const {
    for (int i = 0; i < resolution; ++i) {
        double u0 = double(i) / resolution;
        double u1 = double(i + 1) / resolution;
        for (int j = 0; j < resolution; ++j) {
            double v0 = double(j) / resolution;
            double v1 = double(j + 1) / resolution;

            Point p00 = evaluate(u0, v0);
            Point p10 = evaluate(u1, v0);
            Point p01 = evaluate(u0, v1);
            Point p11 = evaluate(u1, v1);

            // Dois triângulos por quadrado da malha
            outTriangles.emplace_back(p00, p10, p11, material, this);
            outTriangles.emplace_back(p00, p11, p01, material, this);
        }
    }
}

// Método build: tesselação com cache
void BezierSurface::build() const {
    if (!built) {
        generateTriangles(cached_triangles, 20);  // Tessela apenas uma vez
        built = true;
    }
}

// Método hit com cache de tesselação
RT::Trace BezierSurface::hit(const Ray& ray) const {
    build();

    bool hit_any = false;
    double closest_t = std::numeric_limits<double>::max();
    Point hit_position {};
    Vector hit_normal {};
    int hit_index = -1;

    for (size_t i = 0; i < cached_triangles.size(); ++i) {
        RT::Trace result = cached_triangles[i].hit(ray);
        if (result.hit && result.t < closest_t) {
            hit_any = true;
            closest_t = result.t;
            hit_position = result.position;
            hit_normal = result.normal;
            hit_index = static_cast<int>(i);
        }
    }

    if (hit_any) {
        return RT::Trace{ true, closest_t, ray.origin, hit_position, hit_normal.normalized(), this, hit_index };
    }

    return RT::Trace{ false, 0, ray.origin, {}, {}, this, -1 };
}

}

