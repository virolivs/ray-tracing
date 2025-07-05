#include <cmath>
#include "geometry.h"
#include <memory>

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
        double r = radius;

        double a = dot(d, d);
        double b = 2.0f * dot(o, d);
        double c = dot(o, o) - r * r;
        double discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0.0f)
        {
            return RT::Trace { hit, 0.0f, origin, position, normal, this->color };
        }

        double t {};
        double t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
        double t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

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
            return RT::Trace { hit, 0.0f, origin, position, normal, this->color };
        }

        hit = true;
        position = ray.at(t);
        normal = (position - center).normalized();

        return RT::Trace { hit, t, origin, position, normal, this->color };

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

        constexpr double epsilon = 1e-6f;

        if (std::abs(dot(n, d)) < epsilon)
        {
            return RT::Trace { hit, 0.0f, origin, position, normal, this->color };
        }

        double t = dot(n, p - o) / dot(n, d);

        if (t < 0.0f)
        {
            return RT::Trace { hit, 0.0f, origin, position, normal, this->color };
        }

        hit = true;
        position = ray.at(t);
        normal = n.normalized();

        return RT::Trace { hit, t, origin, position, normal, this->color };
    }

    RT::Trace Triangle::hit(const Ray& ray) const
    {
        Vector normal = cross(v1 - v0, v2 - v0).normalized();
        double denom = dot(normal, ray.direction);

        double epsilon = 1e-6;

        if (std::abs(denom) <= epsilon)
            return RT::Trace(false, 0, ray.origin, Point{}, Vector{}, this->color);

        double t = dot(normal, v0 - ray.origin) / denom;

        if (t < 0)
            return RT::Trace(false, 0, ray.origin, Point{}, Vector{}, this->color);

        Point p = ray.at(t);
        Vector u = v1 - v0;
        Vector v = v2 - v0;
        Vector w = p - v0;

        double uu = dot(u, u);
        double uv = dot(u, v);
        double vv = dot(v, v);
        double wu = dot(w, u);
        double wv = dot(w, v);

        double D = uu * vv - uv * uv;

        if (D != 0)
        {
            double alfa = (vv * wu - uv * wv) / D;
            double gamma = (uu * wv - uv * wu) / D;

            if (alfa >= 0 && gamma >= 0 && (alfa + gamma) <= 1)
            {
                return RT::Trace {
                    true,
                    t,
                    ray.origin,
                    p,
                    normal,
                    this->color
                };
            }
        }

        return RT::Trace(false, 0, ray.origin, Point{}, Vector{}, this->color);
    }


    RT::Trace Mesh::hit(const Ray& ray) const
    {
        bool hit_any = false;
        double closest_t = std::numeric_limits<double>::max();
        Point hit_position {};
        Vector hit_normal {};
        Vector hit_color {};

        for (size_t i = 0; i < indices.size(); ++i)
        {
            const auto& tri = indices[i];
            const Point& a = vertices[tri[0]];
            const Point& b = vertices[tri[1]];
            const Point& c = vertices[tri[2]];
            const Vector& color = face_colors[i];

            Triangle temp(a, b, c, color);
            RT::Trace result = temp.hit(ray);

            if (result.hit && result.t < closest_t)
            {
                hit_any = true;
                closest_t = result.t;
                hit_position = result.position;
                hit_normal = result.normal;
                hit_color = result.color;
            }
        }

        if (hit_any)
        {
            return RT::Trace{ true, closest_t, ray.origin, hit_position, hit_normal, hit_color };
        }

        return RT::Trace{ false, 0, ray.origin, {}, {}, Vector{0, 0, 0} };
    }


    Mesh::Mesh(objReader& reader, const Vector& color) : Hittable(color)
    {
        this->vertices = reader.getVertices();
        const auto& faces = reader.getFaces();

        indices.reserve(faces.size());
        triangle_normals.reserve(faces.size());

        vertex_normals.resize(vertices.size(), Vector(0, 0, 0));
        std::vector<int> counts(vertices.size(), 0);

        for (const auto& face : faces) {
            std::array<int, 3> tri = { face.verticeIndice[0], face.verticeIndice[1], face.verticeIndice[2] };
            indices.push_back(tri);

            const Point& a = vertices[tri[0]];
            const Point& b = vertices[tri[1]];
            const Point& c = vertices[tri[2]];

            Vector normal = cross(b - a, c - a).normalized();
            triangle_normals.push_back(normal);

            face_colors.push_back(face.kd);

            for (int idx : tri) {
                vertex_normals[idx] += normal;
                counts[idx]++;
            }
        }

        for (size_t i = 0; i < vertex_normals.size(); ++i) {
            if (counts[i] > 0) {
                vertex_normals[i] = (vertex_normals[i] / double(counts[i])).normalized();
            }
        }
    }


    std::shared_ptr<Mesh> transformMesh(const Mesh& original, const Matrix& transform) 
    {
        auto new_mesh = std::make_shared<Geometry::Mesh>(original);

        for (size_t i = 0; i < new_mesh->vertices.size(); ++i) {
            new_mesh->vertices[i] = transform.applyToPoint(new_mesh->vertices[i]);
        }

        for (size_t i = 0; i < new_mesh->vertex_normals.size(); ++i) {
            new_mesh->vertex_normals[i] = transform.applyToVector(new_mesh->vertex_normals[i]).normalized();
        }

        return new_mesh;
    }

}