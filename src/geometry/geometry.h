#pragma once

#include <vector>
#include <array>
#include "../lib/point.h"
#include "../lib/ray.h"
#include "../lib/vector.h"
#include "../raytracer/trace.h"
#include "../utils/ObjReader.cpp" 
#include "hittable.h"

namespace Geometry
{
    class Sphere : public Hittable
    {
    public:
        Point center {};
        float radius {};

        explicit Sphere(Point center, float radius, Vector color) : Hittable(color), center(center), radius(radius) {}

        Sphere() = default;
        Sphere(const Sphere&) = default;
        ~Sphere() = default;
        Sphere& operator=(const Sphere&) = default;

        RT::Trace hit(const Ray& ray) const override;
    };

    class Plane : public Hittable
    {
    public:
        Point point {};
        Vector normal {};

        explicit Plane(Point point, Vector normal, Vector color) : Hittable(color), point(point), normal(normal) {}

        Plane() = default;
        Plane(const Plane&) = default;
        ~Plane() = default;
        Plane& operator=(const Plane&) = default;

        RT::Trace hit(const Ray& ray) const override;
    };

    class Triangle : public Hittable
    {
    public:
        Point a, b, c {};

        explicit Triangle(Point a, Point b, Point c, Vector color) : Hittable(color), a(a), b(b), c(c) {}

        Triangle() = default;
        Triangle(const Triangle&) = default;
        ~Triangle() = default;
        Triangle& operator=(const Triangle&) = default;

        RT::Trace hit(const Ray& ray) const override;
    };
    
    class Mesh : public Hittable
    {
    public:
        std::vector<Point> vertices;
        std::vector<std::array<int, 3>> indices;
        std::vector<Vector> triangle_normals;
        std::vector<Vector> vertex_normals;

        explicit Mesh(const std::vector<Point>& vertices, const std::vector<std::array<int, 3>>& indices, Vector color);

        explicit Mesh(objReader& reader, Vector color);

        Mesh() = default;
        Mesh(const Mesh&) = default;
        ~Mesh() = default;
        Mesh& operator=(const Mesh&) = default;

        RT::Trace hit(const Ray& ray) const override;
    };
}
