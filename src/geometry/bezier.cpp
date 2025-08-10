#include "bezier.h"
#include <cmath>        
#include <limits>      
#include <algorithm>

namespace Geometry {
static inline void bernstein_all(int n, double t, std::vector<double>& B) {
    B.assign(n + 1, 0.0); 
    B[0] = 1.0;            
    const double u = 1.0 - t; 

    for (int r = 1; r <= n; ++r) {
        double prev = 0.0;
        for (int i = 0; i <= r; ++i) {
            double left  = (i < r) ? B[i] * u : 0.0;
            double right = (i > 0) ? prev * t : 0.0;
            prev = B[i]; 
            B[i] = left + right;
        }
    }
}

static inline bool aabb_hit(const Point& bmin, const Point& bmax, const Ray& r) {
    double tmin = 0.0;
    double tmax = std::numeric_limits<double>::infinity();

    for (int axis = 0; axis < 3; ++axis) {
        double o = axis == 0 ? r.origin.x : (axis == 1 ? r.origin.y : r.origin.z);
        double d = axis == 0 ? r.direction.x : (axis == 1 ? r.direction.y : r.direction.z);
        double invD = (d != 0.0) ? (1.0 / d) : std::numeric_limits<double>::infinity();

        double slab_min = (axis == 0 ? bmin.x : (axis == 1 ? bmin.y : bmin.z));
        double slab_max = (axis == 0 ? bmax.x : (axis == 1 ? bmax.y : bmax.z));

        double t0 = (slab_min - o) * invD;
        double t1 = (slab_max - o) * invD;

        if (invD < 0.0) std::swap(t0, t1); 

        tmin = std::max(tmin, t0);
        tmax = std::min(tmax, t1);

        if (tmax < tmin) return false; 
    }
    return true;
}

BezierSurface::BezierSurface(
    const std::vector<std::vector<Point>>& control_points,
    int degree_u,
    int degree_v,
    const Material& material
) : Hittable(material), control_points(control_points),
    degree_u(degree_u), degree_v(degree_v) {}


static double bernstein(int i, int n, double t) {

    auto binomial = [](int n, int k) {
        double res = 1.0;
        for (int j = 1; j <= k; ++j)
            res *= double(n - j + 1) / j;
        return res;
    };

    if (i < 0 || i > n) return 0.0; 
    return binomial(n, i) * std::pow(t, i) * std::pow(1 - t, n - i);
}


Point BezierSurface::evaluate(double u, double v) const {
    Point p(0, 0, 0);
    for (int i = 0; i <= degree_u; ++i) {
        for (int j = 0; j <= degree_v; ++j) {
            double bu = bernstein(i, degree_u, u);
            double bv = bernstein(j, degree_v, v);
            p += (control_points[i][j] - Point(0, 0, 0)) * (bu * bv);
        }
    }
    return p;
}


std::pair<Vector, Vector> BezierSurface::evaluate_derivatives(double u, double v) const {
    Vector du(0, 0, 0);
    Vector dv(0, 0, 0);

    std::vector<double> Bu_u, Bu, Bv, Bv_v;
    if (degree_u > 0) bernstein_all(degree_u - 1, u, Bu_u);
    bernstein_all(degree_u, u, Bu);
    bernstein_all(degree_v, v, Bv);
    if (degree_v > 0) bernstein_all(degree_v - 1, v, Bv_v);

    if (degree_u > 0) {
        for (int i = 0; i < degree_u; ++i) {
            double wi = Bu_u[i];
            for (int j = 0; j <= degree_v; ++j) {
                double w = wi * Bv[j];
                Vector pij1 = (control_points[i+1][j] - Point(0,0,0));
                Vector pij  = (control_points[i][j]   - Point(0,0,0));
                du += (pij1 - pij) * (double(degree_u) * w);
            }
        }
    }

    if (degree_v > 0) {
        for (int i = 0; i <= degree_u; ++i) {
            double wi = Bu[i];
            for (int j = 0; j < degree_v; ++j) {
                double w = wi * Bv_v[j];
                Vector pij1 = (control_points[i][j+1] - Point(0,0,0));
                Vector pij  = (control_points[i][j]   - Point(0,0,0));
                dv += (pij1 - pij) * (double(degree_v) * w);
            }
        }
    }

    return {du, dv};
}


Vector BezierSurface::normal_at(double u, double v) const {
    auto [du, dv] = evaluate_derivatives(u, v);
    return cross(du, dv).normalized();
}


void BezierSurface::generateTriangles(std::vector<Triangle>& outTriangles, int resolution) const {
    const int R = std::max(2, resolution);

    std::vector<std::vector<double>> Bu_all(R + 1), Bv_all(R + 1);
    std::vector<std::vector<double>> Bu_all_du(R + 1), Bv_all_dv(R + 1);

    for (int k = 0; k <= R; ++k) {
        double u = double(k) / R;
        double v = double(k) / R;

        bernstein_all(degree_u, u, Bu_all[k]);
        bernstein_all(degree_v, v, Bv_all[k]);

        if (degree_u > 0) bernstein_all(degree_u - 1, u, Bu_all_du[k]);
        if (degree_v > 0) bernstein_all(degree_v - 1, v, Bv_all_dv[k]);
    }

    std::vector<std::vector<Point>>  gridP(R + 1, std::vector<Point>(R + 1));
    std::vector<std::vector<Vector>> gridN(R + 1, std::vector<Vector>(R + 1));

    for (int ku = 0; ku <= R; ++ku) {
        for (int kv = 0; kv <= R; ++kv) {
            Vector acc(0,0,0); 
            for (int i = 0; i <= degree_u; ++i) {
                for (int j = 0; j <= degree_v; ++j) {
                    double w = Bu_all[ku][i] * Bv_all[kv][j];
                    acc += (control_points[i][j] - Point(0,0,0)) * w;
                }
            }
            gridP[ku][kv] = Point(acc.x, acc.y, acc.z);


            Vector du(0,0,0), dv(0,0,0);

            if (degree_u > 0) {
                for (int i = 0; i < degree_u; ++i) {
                    for (int j = 0; j <= degree_v; ++j) {
                        double w = Bu_all_du[ku][i] * Bv_all[kv][j];
                        Vector pij1 = (control_points[i+1][j] - Point(0,0,0));
                        Vector pij  = (control_points[i][j]   - Point(0,0,0));
                        du += (pij1 - pij) * (double(degree_u) * w);
                    }
                }
            }

            if (degree_v > 0) {
                for (int i = 0; i <= degree_u; ++i) {
                    for (int j = 0; j < degree_v; ++j) {
                        double w = Bu_all[ku][i] * Bv_all_dv[kv][j];
                        Vector pij1 = (control_points[i][j+1] - Point(0,0,0));
                        Vector pij  = (control_points[i][j]   - Point(0,0,0));
                        dv += (pij1 - pij) * (double(degree_v) * w);
                    }
                }
            }

            gridN[ku][kv] = cross(du, dv).normalized();
        }
    }

    for (int i = 0; i < R; ++i) {
        int i0 = i, i1 = i + 1;
        for (int j = 0; j < R; ++j) {
            int j0 = j, j1 = j + 1;

            const Point& p00 = gridP[i0][j0];
            const Point& p10 = gridP[i1][j0];
            const Point& p01 = gridP[i0][j1];
            const Point& p11 = gridP[i1][j1];

            const Vector& n00 = gridN[i0][j0];
            const Vector& n10 = gridN[i1][j0];
            const Vector& n01 = gridN[i0][j1];
            const Vector& n11 = gridN[i1][j1];

            Triangle t1(p00, p10, p11, material, this);
            Triangle t2(p00, p11, p01, material, this);

            t1.cached_normal = (n00 + n10 + n11).normalized();
            t2.cached_normal = (n00 + n11 + n01).normalized();

            outTriangles.push_back(t1);
            outTriangles.push_back(t2);
        }
    }
}


void BezierSurface::build() const {
    if (!built) { 
        generateTriangles(cached_triangles, 64); //
        built = true; 
    }
}


RT::Trace BezierSurface::hit(const Ray& ray) const {
    Point bmin(+std::numeric_limits<double>::infinity(), 
               +std::numeric_limits<double>::infinity(), 
               +std::numeric_limits<double>::infinity());
    Point bmax(-std::numeric_limits<double>::infinity(), 
               -std::numeric_limits<double>::infinity(), 
               -std::numeric_limits<double>::infinity());

    for (int i = 0; i <= degree_u; ++i) { 
        for (int j = 0; j <= degree_v; ++j) { 
            const Point& p = control_points[i][j]; 
            bmin.x = std::min(bmin.x, p.x);
            bmin.y = std::min(bmin.y, p.y); 
            bmin.z = std::min(bmin.z, p.z); 
            bmax.x = std::max(bmax.x, p.x); 
            bmax.y = std::max(bmax.y, p.y);
            bmax.z = std::max(bmax.z, p.z); 
        }
    }

    if (!aabb_hit(bmin, bmax, ray))
        return RT::Trace{ false, 0, ray.origin, {}, {}, this, -1 };

    build(); 

    bool hit_any = false; 
    double closest_t = std::numeric_limits<double>::max(); 
    Point  hit_position{};
    Vector hit_normal{}; 
    int    hit_index = -1; 

    for (size_t i = 0; i < cached_triangles.size(); ++i) { 
        RT::Trace result = cached_triangles[i].hit(ray); 
        if (result.hit && result.t < closest_t) { 
            hit_any      = true; 
            closest_t    = result.t; 
            hit_position = result.position; 
            hit_normal   = cached_triangles[i].cached_normal;
            hit_index    = static_cast<int>(i);
        }
    }

    if (hit_any) { 
        Vector n = hit_normal.normalized(); 
        if (dot(n, ray.direction) > 0.0) n = -n; 
        return RT::Trace{ true, closest_t, ray.origin, hit_position, n, this, hit_index };
    }

    return RT::Trace{ false, 0, ray.origin, {}, {}, this, -1 }; 
}


const Material& BezierSurface::get_material(int face_index) const {
    return material; 
}

} 