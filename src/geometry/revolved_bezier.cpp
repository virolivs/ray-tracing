#include "revolved_bezier.h"
#include <cmath>

namespace Geometry {

RevolvedBezierSurface::RevolvedBezierSurface(
    const std::vector<Point>& control_points,
    int degree,
    const Material& material
) : Hittable(material), control_points(control_points), degree(degree) {}

Point RevolvedBezierSurface::evaluate_curve(double u) const {
    Point p(0, 0, 0);
    for (int i = 0; i <= degree; ++i) {
        double binom = 1;
        for (int j = 1; j <= i; ++j)
            binom *= double(degree - j + 1) / j;
        double B = binom * pow(u, i) * pow(1 - u, degree - i);
        p += (control_points[i] - Point(0, 0, 0)) * B;
    }
    return p;
}

Point RevolvedBezierSurface::surface_point(double u, double theta) const {
    Point p = evaluate_curve(u); // p.x = raio, p.y = altura
    return Point(p.x * cos(theta), p.y, p.x * sin(theta));
}

void RevolvedBezierSurface::generateTriangles(std::vector<Triangle>& out, int u_res, int theta_res) const {
    for (int i = 0; i < u_res; ++i) {
        double u0 = double(i) / u_res;
        double u1 = double(i + 1) / u_res;

        for (int j = 0; j < theta_res; ++j) {
            double t0 = (double(j) / theta_res) * 2 * M_PI;
            double t1 = (double(j + 1) / theta_res) * 2 * M_PI;

            Point p00 = surface_point(u0, t0);
            Point p10 = surface_point(u1, t0);
            Point p01 = surface_point(u0, t1);
            Point p11 = surface_point(u1, t1);
            
            Vector normal = cross(p10 - p00, p11 - p00);
            if (normal.norm() == 0.0)
                continue;  // ou use um fallback como Vector(0,1,0)
            Vector n = normal.normalized();



            Triangle tri1(p00, p10, p11, material);
            Triangle tri2(p00, p11, p01, material);

            out.push_back(tri1);
            out.push_back(tri2);
        }
    }
}

void RevolvedBezierSurface::build() const {
    if (!built) {
        generateTriangles(cached_triangles, this->u_resolution, this->theta_resolution);
        built = true;
    }
}

RT::Trace RevolvedBezierSurface::hit(const Ray& ray) const {
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

            const Point& a = cached_triangles[i].v0;
            const Point& b = cached_triangles[i].v1;
            const Point& c = cached_triangles[i].v2;
            hit_normal = cross(b - a, c - a).normalized();

            hit_index = static_cast<int>(i);
        }
    }

    if (hit_any) {
        return RT::Trace{ true, closest_t, ray.origin, hit_position, hit_normal, this, hit_index };
    }

    return RT::Trace{ false, 0, ray.origin, {}, {}, this, -1 };
}

}
