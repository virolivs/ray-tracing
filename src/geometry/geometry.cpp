#include <cmath>
#include "geometry.h"

namespace Geometry
{
    RT::Trace Sphere::hit(const Ray& ray) const
    {
        bool hit { false };
        Point origin { ray.origin };
        Point position {};
        Vector normal {};

        Vector o = ray.origin - center;
        Vector d = ray.direction;
        float r = radius;

        float a = dot(d, d);
        float b = 2.0f * dot(o, d);
        float c = dot(o, o) - r * r;
        float discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0.0f)
        {
            return RT::Trace { hit, 0.0f, origin, position, normal };
        }

        float t {};
        float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
        float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

        if (t1 > 0.0f)
        {
            t = t1;
        }
        else if (t2 > 0.0f)
        {
            t = t2;
        }
        else
        {
            return RT::Trace { hit, 0.0f, origin, position, normal };
        }

        hit = true;
        position = ray.at(t);
        normal = (position - center).normalized();

        return RT::Trace { hit, t, origin, position, normal };
    }

    RT::Trace Plane::hit(const Ray& ray) const
    {
        bool hit { false };
        Point origin { ray.origin };
        Point position {};
        Vector normal {};

        Point o = ray.origin;
        Vector d = ray.direction;
        Point p = this->point;
        Vector n = this->normal;

        constexpr float epsilon = 1e-6f;

        if (std::abs(dot(n, d)) < epsilon)
        {
            return RT::Trace { hit, 0.0f, origin, position, normal };
        }

        float t = dot(n, p - o) / dot(n, d);

        if (t < 0.0f)
        {
            return RT::Trace { hit, 0.0f, origin, position, normal };
        }

        hit = true;
        position = ray.at(t);
        normal = n.normalized();

        return RT::Trace { hit, t, origin, position, normal };
    }

    RT::Trace Triangle::hit(const Ray& ray) const
    {
        // TODO: Implement the hit function for Triangle

        bool hit { false };
        Point origin { ray.origin };
        Point position {};
        Vector normal {};
        return RT::Trace { hit, 0.0f, origin, position, normal };
    }

    RT::Trace Mesh::hit(const Ray& ray) const
    // Performs ray-triangle intersection tests against all triangles in the mesh.
    // For each triangle, it checks if the ray intersects its plane and then
    // uses barycentric coordinates to determine if the intersection point lies inside the triangle.
    {
        bool hit_any = false;
        float closest_t = std::numeric_limits<float>::max();
        Point hit_position {};
        Vector hit_normal {};

        for (size_t i = 0; i < indices.size(); ++i)
        {
            const auto& [i0, i1, i2] = indices[i];
            const Point& v0 = vertices[i0];
            const Point& v1 = vertices[i1];
            const Point& v2 = vertices[i2];
            const Vector& normal = triangle_normals[i];

            // Calculate the denominator of the intersection formula (dot product between triangle normal and ray direction)
            float denom = dot(normal, ray.direction);

            // If denom is near zero, the ray is parallel to the triangle plane, so skip this triangle
            if (std::abs(denom) < 1e-6f) continue; // ## check this later!!!! 

            // Calculate the parameter t for the ray-plane intersection
            float t = dot(normal, v0 - ray.origin) / denom;

            // If t is negative, the intersection is behind the ray origin; if t >= closest_t, 
            // we've already found a closer intersection, so skip this one
            if (t < 0 || t >= closest_t) continue;

            Point p = ray.at(t);

            Vector u = v1 - v0;
            Vector v = v2 - v0;
            Vector w = p - v0;

            float uu = dot(u, u);
            float uv = dot(u, v);
            float vv = dot(v, v);
            float wu = dot(w, u);
            float wv = dot(w, v);
            float D = uv * uv - uu * vv;

            float s = (uv * wv - vv * wu) / D;
            float t_bary = (uv * wu - uu * wv) / D;
            
            // Barycentric condition
            if (s >= 0 && t_bary >= 0 && (s + t_bary) <= 1)
            {
                hit_any = true;
                closest_t = t;
                hit_position = p;
                hit_normal = normal.normalized();
            }
        }

        if (hit_any)
        {
            return RT::Trace {
                true,
                closest_t,
                ray.origin,
                hit_position,
                hit_normal
            };
        }

        return RT::Trace { false, 0, ray.origin, {}, {} };
    }


Mesh::Mesh(objReader& reader, Vector color)
        : Hittable(color)
    {
        this->vertices = reader.getVertices();
        const auto& faces = reader.getFaces();

        for (const auto& face : faces) {
            std::array<int, 3> tri = {
                face.verticeIndice[0],
                face.verticeIndice[1],
                face.verticeIndice[2]
            };
            indices.push_back(tri);

            const Point& a = vertices[tri[0]];
            const Point& b = vertices[tri[1]];
            const Point& c = vertices[tri[2]];

            Vector normal = cross(b - a, c - a).normalized();
            triangle_normals.push_back(normal);
        }
    }
}