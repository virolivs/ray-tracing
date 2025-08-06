#pragma once

#include <vector>
#include <array>
#include "../lib/point.h"
#include "../lib/ray.h"
#include "../lib/vector.h"
#include "../raytracer/trace.h"
#include "../utils/ObjReader.cpp"
#include "hittable.h"
#include "../matrix/matrix.h"
#include "material.h"

namespace Geometry
{
    class Sphere : public Hittable
    {
    public:
        Point center {};
        double radius {};

        explicit Sphere(Point center, double radius, Material material)
            : Hittable(material), center(center), radius(radius) {}

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

        explicit Plane(Point point, Vector normal, Material material)
            : Hittable(material), point(point), normal(normal) {}

        Plane() = default;
        Plane(const Plane&) = default;
        ~Plane() = default;
        Plane& operator=(const Plane&) = default;

        RT::Trace hit(const Ray& ray) const override;
    };

    class Triangle : public Hittable
    {
    public:
        Point v0, v1, v2;
        const Hittable* parent = nullptr;
        Vector cached_normal = Vector(0, 1, 0);

        explicit Triangle(const Point& a, const Point& b, const Point& c, const Material& material)
            : Hittable(material), v0(a), v1(b), v2(c) {}

        explicit Triangle(const Point& a, const Point& b, const Point& c, const Material& material, const Hittable* parent)
            : Hittable(material), v0(a), v1(b), v2(c), parent(parent) {}
    

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
        std::vector<Vector> face_colors;
        std::vector<Material> materials;

        explicit Mesh(objReader& reader);

        Mesh() = default;
        Mesh(const Mesh&) = default;
        ~Mesh() = default;
        Mesh& operator=(const Mesh&) = default;

        RT::Trace hit(const Ray& ray) const override;

    };

    std::shared_ptr<Mesh> transformMesh(const Mesh& original, const Matrix& transform);
    
}
