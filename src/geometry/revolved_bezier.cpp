#include "revolved_bezier.h"
#include <cmath>
#include <vector>

namespace Geometry {

    RevolvedBezierSurface::RevolvedBezierSurface(
        const std::vector<Point>& control_points,
        int degree,
        const Material& material
    ) : Hittable(material), control_points(control_points), degree(degree) {
        tmp_work.reserve(control_points.size());
    }

    Point RevolvedBezierSurface::evaluate_curve(double u) const {
        tmp_work = control_points;
        const int n = degree;
        for (int r = 1; r <= n; ++r) {
            for (int i = 0; i <= n - r; ++i) {
                tmp_work[i] = tmp_work[i] + (tmp_work[i + 1] - tmp_work[i]) * u;
            }
        }
        return tmp_work[0];
    }

    Point RevolvedBezierSurface::surface_point(double u, double theta) const {
        Point p = evaluate_curve(u);
        return Point(p.x * std::cos(theta), p.y, p.x * std::sin(theta));
    }

    void RevolvedBezierSurface::generateTriangles(std::vector<Triangle>& out, int u_res, int theta_res) const {
        out.reserve(out.size() + static_cast<size_t>(2) * u_res * theta_res);

        const double two_pi = 2.0 * M_PI;
        std::vector<double> cos_t(theta_res + 1), sin_t(theta_res + 1);
        for (int j = 0; j <= theta_res; ++j) {
            double t = (static_cast<double>(j) / theta_res) * two_pi;
            cos_t[j] = std::cos(t);
            sin_t[j] = std::sin(t);
        }

        constexpr double EPS2 = 1e-24;

        for (int i = 0; i < u_res; ++i) {
            const double u0 = static_cast<double>(i) / u_res;
            const double u1 = static_cast<double>(i + 1) / u_res;

            const Point pu0 = evaluate_curve(u0);
            const Point pu1 = evaluate_curve(u1);

            const double r0 = pu0.x, y0 = pu0.y;
            const double r1 = pu1.x, y1 = pu1.y;

            for (int j = 0; j < theta_res; ++j) {
                const int j0 = j;
                const int j1 = j + 1;

                const Point p00(r0 * cos_t[j0], y0, r0 * sin_t[j0]);
                const Point p10(r1 * cos_t[j0], y1, r1 * sin_t[j0]);
                const Point p01(r0 * cos_t[j1], y0, r0 * sin_t[j1]);
                const Point p11(r1 * cos_t[j1], y1, r1 * sin_t[j1]);

                Vector n = cross(p10 - p00, p11 - p00);
                if (n.norm_sqr() < EPS2) continue;
                n.normalize();

                out.emplace_back(p00, p10, p11, material);
                out.back().cached_normal = n;

                out.emplace_back(p00, p11, p01, material);
                out.back().cached_normal = n;
            }
        }
    }

    void RevolvedBezierSurface::build() const {
        if (!built) {
            cached_triangles.clear();
            cached_triangles.reserve(static_cast<size_t>(2) * u_resolution * theta_resolution);
            generateTriangles(cached_triangles, u_resolution, theta_resolution);
            built = true;
        }
    }

    RT::Trace RevolvedBezierSurface::hit(const Ray& ray) const {
        build();

        bool hit_any = false;
        double closest_t = std::numeric_limits<double>::infinity();
        Point  hit_position{};
        Vector hit_normal{};
        int    hit_index = -1;

        for (size_t i = 0; i < cached_triangles.size(); ++i) {
            const Triangle& tri = cached_triangles[i];
            RT::Trace result = tri.hit(ray);
            if (!result.hit || result.t >= closest_t) continue;

            hit_any = true;
            closest_t = result.t;
            hit_position = result.position;

            Vector n = tri.cached_normal;
            if (n.norm_sqr() == 0.0) {
                n = cross(tri.v1 - tri.v0, tri.v2 - tri.v0).normalized();
            }
            hit_normal = n;
            hit_index = static_cast<int>(i);
        }

        if (hit_any) {
            return RT::Trace{ true, closest_t, ray.origin, hit_position, hit_normal, this, hit_index };
        }
        return RT::Trace{ false, 0.0, ray.origin, {}, {}, this, -1 };
    }

}
