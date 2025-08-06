#pragma once

#include <vector>
#include "../lib/point.h"
#include "../lib/vector.h"
#include "../lib/ray.h"
#include "../raytracer/trace.h"
#include "geometry.h"
#include "material.h"
#include "hittable.h"

namespace Geometry {

class BezierSurface : public Hittable {
public:
    std::vector<std::vector<Point>> control_points;
    int degree_u;
    int degree_v;

    BezierSurface(
        const std::vector<std::vector<Point>>& control_points,
        int degree_u,
        int degree_v,
        const Material& material
    );

    Point evaluate(double u, double v) const;
    RT::Trace hit(const Ray& ray) const override;

    const Material& get_material(int face_index = -1) const override;

private:
    void build() const;
    
    std::pair<Vector, Vector> evaluate_derivatives(double u, double v) const;
    Vector normal_at(double u, double v) const;

    void generateTriangles(std::vector<Triangle>& outTriangles, int resolution) const;

    mutable std::vector<Triangle> cached_triangles;
    mutable bool built = false;
};

}

