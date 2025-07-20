#pragma once

#include "../lib/point.h"
#include "../lib/vector.h"

class Hittable;

namespace RT
{
    struct Trace
    {
        bool hit {};
        double t {};
        Point origin {};
        Point position {};
        Vector normal {};
        const Hittable* hittable {};
        int face_index {-1};

        Trace() = default;

        Trace(bool hit, double t, const Point& origin, const Point& position,
              const Vector& normal, const Hittable* hittable, int face_index = -1)
            : hit(hit), t(t), origin(origin), position(position), normal(normal),
              hittable(hittable), face_index(face_index) {}

        Trace(const Trace&) = default;
        Trace& operator=(const Trace&) = default;
        ~Trace() = default;
    };
}

inline std::ostream& operator<<(std::ostream& os, const RT::Trace& t)
{
    os << "Trace(" << std::boolalpha << t.hit << ", "
       << t.origin << ", "
       << t.position << ", "
       << t.normal << ")";
    return os;
}
