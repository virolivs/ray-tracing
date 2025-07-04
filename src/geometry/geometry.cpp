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
        float r = radius;

        float a = dot(d, d);
        float b = 2.0f * dot(o, d);
        float c = dot(o, o) - r * r;
        float discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0.0f)
        {
            return RT::Trace { hit, 0.0f, origin, position, normal, this->color };
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

        constexpr float epsilon = 1e-6f;

        if (std::abs(dot(n, d)) < epsilon)
        {
            return RT::Trace { hit, 0.0f, origin, position, normal, this->color };
        }

        float t = dot(n, p - o) / dot(n, d);

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
        // Compute the face normal using the cross product of two edges of the triangle
        Vector normal = cross(v1 - v0, v2 - v0).normalized();

        // Calculate the dot product between the triangle normal and the ray direction
        float denom = dot(normal, ray.direction);

        // If the ray is nearly parallel to the triangle plane, no hit
        if (std::abs(denom) < 1e-6f)
            return RT::Trace(false, 0, ray.origin, Point{}, Vector{}, this->color);

        // Calculate distance t along the ray where it intersects the triangle plane
        float t = dot(normal, v0 - ray.origin) / denom;

        // If intersection is behind the ray origin, no hit
        if (t < 0)
            return RT::Trace(false, 0, ray.origin, Point{}, Vector{}, this->color);

        // Compute the intersection point on the plane
        Point p = ray.at(t);

        // Prepare vectors for barycentric coordinate calculation
        Vector u = v1 - v0;
        Vector v = v2 - v0;
        Vector w = p - v0;

        // Compute dot products needed for barycentric coordinates
        float uu = dot(u, u);
        float uv = dot(u, v);
        float vv = dot(v, v);
        float wu = dot(w, u);
        float wv = dot(w, v);

        // Compute the denominator of the barycentric coordinate formula
        float D = uv * uv - uu * vv;

        // Compute barycentric coordinates s and t_bary
        float s = (uv * wv - vv * wu) / D;
        float t_bary = (uv * wu - uu * wv) / D;

        // Check if point lies inside the triangle using barycentric conditions
        if (s >= 0 && t_bary >= 0 && (s + t_bary) <= 1)
        {
            // If inside, return a successful hit record with intersection details and color
            return RT::Trace {
                true,
                t,
                ray.origin,
                p,
                normal,
                this->color // ← cor associada ao triângulo
            };
        }

        // Otherwise, no hit
        return RT::Trace(false, 0, ray.origin, Point{}, Vector{}, this->color);
    }

    RT::Trace Mesh::hit(const Ray& ray) const
    {
        bool hit_any = false;                                   // Flag to indicate if any triangle was hit
        float closest_t = std::numeric_limits<float>::max();    // Store the smallest t value (closest intersection)
        Point hit_position {};                                  // Closest hit position found
        Vector hit_normal {};                                   // Normal at the hit point
        Vector hit_color {};                                    // Color at the hit point

        // Iterate over all triangles in the mesh
        for (size_t i = 0; i < indices.size(); ++i)
        {
            const Triangle& triangle = triangles[i];            // Access the i-th triangle
            RT::Trace result = triangle.hit(ray);               // Test intersection between ray and triangle

            // If the triangle was hit and the intersection is closer than previous hits
            if (result.hit && result.t < closest_t)
            {
                hit_any = true;                                 // Mark that a hit occurred
                closest_t = result.t;                           // Update closest intersection distance
                hit_position = result.position;                 // Update hit position
                hit_normal = result.normal;                     // Update normal at hit point
                hit_color = result.color;                       // Update color of the hit triangle
            }
        }

        // If any intersection was found, return the closest hit information
        if (hit_any)
        {
            return RT::Trace {
                true,           // hit = true
                closest_t,      // closest intersection distance
                ray.origin,     // origin of the ray
                hit_position,   // position of the hit
                hit_normal,     // normal at the hit
                hit_color       // color at the hit point
            };
        }

        // If no hit occurred, return a Trace indicating no hit with default values
        return RT::Trace { false, 0, ray.origin, {}, {}, Vector{0, 0, 0} };
    }

    Mesh::Mesh(objReader& reader, Vector color) : Hittable(color)
    {
        this->vertices = reader.getVertices();                      // Load mesh vertices from objReader
        const auto& faces = reader.getFaces();                      // Load mesh faces (triangles) from objReader

        indices.reserve(faces.size());                              // Reserve space for triangle indices
        triangle_normals.reserve(faces.size());                     // Reserve space for triangle normals
        vertex_normals.resize(vertices.size(), Vector(0, 0, 0));    // Initialize vertex normals to zero
        std::vector<int> counts(vertices.size(), 0);                // Track how many triangles share each vertex

        // Iterate over each face (triangle) in the mesh
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

            // Triangle storage
            triangles.emplace_back(a, b, c, face.kd);

            for (int idx : tri) {
                vertex_normals[idx] += normal;
                counts[idx]++;
            }
        }

        // Normalize the accumulated vertex normals by averaging
        for (size_t i = 0; i < vertex_normals.size(); ++i) {
            if (counts[i] > 0) {
                vertex_normals[i] = (vertex_normals[i] / float(counts[i])).normalized();
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

        new_mesh->triangles.clear();

        for (size_t i = 0; i < new_mesh->indices.size(); ++i) {
            const auto& idx = new_mesh->indices[i];
            const auto& a = new_mesh->vertices[idx[0]];
            const auto& b = new_mesh->vertices[idx[1]];
            const auto& c = new_mesh->vertices[idx[2]];
            
            Vector tri_color = original.triangles[i].color;

            new_mesh->triangles.emplace_back(a, b, c, tri_color);
        }

        return new_mesh;
    }

}