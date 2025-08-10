#include "revolved_bezier.h"
#include <cmath>
#include <limits>
#include <algorithm>

namespace Geometry {


static inline bool aabb_hit(const Point& bmin, const Point& bmax, const Ray& r) {
    double tmin = 0.0;
    double tmax = std::numeric_limits<double>::infinity();     

    for (int axis = 0; axis < 3; ++axis) {
        double o = (axis == 0) ? r.origin.x : (axis == 1) ? r.origin.y : r.origin.z;
        double d = (axis == 0) ? r.direction.x : (axis == 1) ? r.direction.y : r.direction.z;

        double invD = (d != 0.0) ? (1.0 / d) : std::numeric_limits<double>::infinity();

        double slab_min = (axis == 0) ? bmin.x : (axis == 1) ? bmin.y : bmin.z;
        double slab_max = (axis == 0) ? bmax.x : (axis == 1) ? bmax.y : bmax.z;

        double t0 = (slab_min - o) * invD;
        double t1 = (slab_max - o) * invD;

        if (invD < 0.0) std::swap(t0, t1);

        tmin = std::max(tmin, t0);
        tmax = std::min(tmax, t1);

        if (tmax < tmin) return false;
    }
    return true;
}


RevolvedBezierSurface::RevolvedBezierSurface(
    const std::vector<Point>& control_points,
    int degree,
    const Material& material
) : Hittable(material), control_points(control_points), degree(degree) {}

Point RevolvedBezierSurface::evaluate_curve(double u) const {
    Point p(0, 0, 0);
    for (int i = 0; i <= degree; ++i) {
       
        double binom = 1.0;
        for (int j = 1; j <= i; ++j) binom *= double(degree - j + 1) / j;

        double B = binom * std::pow(u, i) * std::pow(1.0 - u, degree - i);

        p += (control_points[i] - Point(0, 0, 0)) * B;
    }
    return p; // p.x = raio, p.y = altura
}

Point RevolvedBezierSurface::surface_point(double u, double theta) const {
    Point p = evaluate_curve(u);
    return Point(p.x * std::cos(theta), p.y, p.x * std::sin(theta));
}


void RevolvedBezierSurface::generateTriangles(std::vector<Triangle>& out, int u_res, int theta_res) const {
    out.reserve(out.size() + 2 * u_res * theta_res); 
    const double EPS = 1e-12;                        

    for (int i = 0; i < u_res; ++i) {
        double u0 = double(i) / u_res;
        double u1 = double(i + 1) / u_res;

        for (int j = 0; j < theta_res; ++j) {
            double t0 = (double(j) / theta_res) * 2.0 * M_PI;
            double t1 = (double(j + 1) / theta_res) * 2.0 * M_PI;

            Point p00 = surface_point(u0, t0);
            Point p10 = surface_point(u1, t0);
            Point p01 = surface_point(u0, t1);
            Point p11 = surface_point(u1, t1);

            {
                Vector e1 = p10 - p00;
                Vector e2 = p11 - p00;
                Vector n  = cross(e1, e2); 
                if (e1.norm() > EPS && e2.norm() > EPS && n.norm() > EPS) {
                    out.emplace_back(p00, p10, p11, material);
                }
            }

            {
                Vector e1 = p11 - p00;
                Vector e2 = p01 - p00;
                Vector n  = cross(e1, e2);
                if (e1.norm() > EPS && e2.norm() > EPS && n.norm() > EPS) {
                    out.emplace_back(p00, p11, p01, material);
                }
            }
        }
    }
}


void RevolvedBezierSurface::build() const {
    if (!built) {
        cached_triangles.clear();
        generateTriangles(cached_triangles, this->u_resolution, this->theta_resolution);
        built = true;
    }
}


RT::Trace RevolvedBezierSurface::hit(const Ray& ray) const {
    double rmax = 0.0;
    double ymin = +std::numeric_limits<double>::infinity();
    double ymax = -std::numeric_limits<double>::infinity();

    for (const auto& cp : control_points) {
        rmax = std::max(rmax, std::abs(cp.x)); 
        ymin = std::min(ymin, cp.y);           
        ymax = std::max(ymax, cp.y);          
    }

    Point bmin(-rmax, ymin, -rmax);
    Point bmax( rmax, ymax,  rmax);

    if (!aabb_hit(bmin, bmax, ray))
        return RT::Trace{ false, 0.0, ray.origin, {}, {}, this, -1 };

    build();

    bool   hit_any   = false;
    double closest_t = std::numeric_limits<double>::max();
    Point  hit_pos{};
    Vector hit_n{};
    int    hit_idx = -1;
    const double EPS = 1e-12;

    for (size_t i = 0; i < cached_triangles.size(); ++i) {
        const Triangle& tri = cached_triangles[i];

        RT::Trace res = tri.hit(ray);
        if (res.hit && res.t < closest_t) {
            hit_any   = true;
            closest_t = res.t;
            hit_pos   = res.position;

            const Point& a = tri.v0;
            const Point& b = tri.v1;
            const Point& c = tri.v2;

            Vector n = cross(b - a, c - a);
            if (n.norm() <= EPS) {
                n = Vector(0, 1, 0);
            } else {
                n = n.normalized();
            }

            if (dot(n, ray.direction) > 0.0) n = -n;

            hit_n   = n;
            hit_idx = static_cast<int>(i);
        }
    }

    if (hit_any) {
        return RT::Trace{ true, closest_t, ray.origin, hit_pos, hit_n, this, hit_idx };
    }
    return RT::Trace{ false, 0.0, ray.origin, {}, {}, this, -1 };
}

}