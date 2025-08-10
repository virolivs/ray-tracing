#pragma once
#include "hittable.h"
#include "material.h"
#include "geometry.h"
#include <vector>
#include <memory>

namespace Geometry {
    class RevolvedBezierSurface : public Hittable {
    public:
        RevolvedBezierSurface(const std::vector<Point>& control_points, int degree, const Material& material);
        RT::Trace hit(const Ray& ray) const override;
        void set_resolution(int u_res, int theta_res) {
            u_resolution = u_res;
            theta_resolution = theta_res;
        }

    private:
        int u_resolution = 40;
        int theta_resolution = 60;
        std::vector<Point> control_points;
        int degree;
        mutable std::vector<Triangle> cached_triangles;
        mutable bool built = false;
        mutable std::vector<Point> tmp_work;

    private:
        void build() const;
        void generateTriangles(std::vector<Triangle>& out, int u_res, int theta_res) const;
        Point evaluate_curve(double u) const;
        Point surface_point(double u, double theta) const;
    };
}
