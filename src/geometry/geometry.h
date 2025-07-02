#pragma once

#include "../lib/point.h"
#include "../lib/ray.h"
#include "../lib/vector.h"
#include "../raytracer/trace.h"

namespace Geometry
{
    class Sphere
    {
    public:
        Point center {};
        float radius {};

        explicit Sphere(Point center, float radius) : center { center }, radius { radius } {}

        Sphere() = default;
        Sphere(const Sphere&) = default;
        ~Sphere() = default;
        Sphere& operator=(const Sphere&) = default;

        RT::Trace hit(const Ray& ray) const;
    };

    class Plane
    {
    public:
        Point point {};
        Vector normal {};

        explicit Plane(Point point, Vector normal) : point { point }, normal { normal } {}

        Plane() = default;
        Plane(const Plane&) = default;
        ~Plane() = default;
        Plane& operator=(const Plane&) = default;

        RT::Trace hit(const Ray& ray) const;
    };

    class Triangle
    {
    public:
        Point a, b, c {};

        explicit Triangle(Point a, Point b, Point c) : a { a }, b { b }, c { c } {}

        Triangle() = default;
        Triangle(const Triangle&) = default;
        ~Triangle() = default;
        Triangle& operator=(const Triangle&) = default;

        RT::Trace hit(const Ray& ray) const;
    };
}
