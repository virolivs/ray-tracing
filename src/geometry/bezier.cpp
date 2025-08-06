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

    if (i < 0 || i > n) return 0.0;
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

// Derivadas parciais para calcular normal suave
std::pair<Vector, Vector> BezierSurface::evaluate_derivatives(double u, double v) const {
    Vector du(0, 0, 0);
    Vector dv(0, 0, 0);

    for (int i = 0; i <= degree_u; ++i) {
        for (int j = 0; j <= degree_v; ++j) {
            double Bu = bernstein(i, degree_u, u);
            double Bv = bernstein(j, degree_v, v);

            double dBu = degree_u == 0 ? 0.0 :
                degree_u * (bernstein(i - 1, degree_u - 1, u) - bernstein(i, degree_u - 1, u));
            double dBv = degree_v == 0 ? 0.0 :
                degree_v * (bernstein(j - 1, degree_v - 1, v) - bernstein(j, degree_v - 1, v));

            const Point& pij = control_points[i][j];

            du += (pij - Point(0, 0, 0)) * (dBu * Bv);
            dv += (pij - Point(0, 0, 0)) * (Bu * dBv);
        }
    }

    return {du, dv};
}

// Normal suave em (u, v)
Vector BezierSurface::normal_at(double u, double v) const {
    auto [du, dv] = evaluate_derivatives(u, v);
    return cross(dv, du).normalized(); ;
}

// Gera os triângulos da tesselação com normais suaves
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

            // Ponto médio do patch
            double u_mid = 0.5 * (u0 + u1);
            double v_mid = 0.5 * (v0 + v1);
            Vector n = normal_at(u_mid, v_mid);

            Triangle t1(p00, p10, p11, material, this);
            Triangle t2(p00, p11, p01, material, this);

            t1.cached_normal = n;
            t2.cached_normal = n;

            outTriangles.push_back(t1);
            outTriangles.push_back(t2);
        }
    }
}

// Método build: tesselação com cache
void BezierSurface::build() const {
    if (!built) {
        generateTriangles(cached_triangles, 20); 
        built = true;
    }
}

// Método hit com uso da normal suave
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
            hit_normal = cached_triangles[i].cached_normal;
            hit_index = static_cast<int>(i);
            //std::cout << "[HIT] normal usada: " << hit_normal.x << ", " << hit_normal.y << ", " << hit_normal.z << "\n";
        }
    }

    if (hit_any) {
        return RT::Trace{ true, closest_t, ray.origin, hit_position, hit_normal.normalized(), this, hit_index };
    }

    return RT::Trace{ false, 0, ray.origin, {}, {}, this, -1 };
}

const Material& BezierSurface::get_material(int face_index) const {
    std::cout << "[GET_MATERIAL] ka: "
              << material.ka.x << ", " << material.ka.y << ", " << material.ka.z
              << " | kd: "
              << material.kd.x << ", " << material.kd.y << ", " << material.kd.z
              << " | ks: "
              << material.ks.x << ", " << material.ks.y << ", " << material.ks.z
              << " | shininess: "
              << material.shininess
              << std::endl;

    return material;
}



}
