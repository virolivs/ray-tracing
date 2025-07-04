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
        // Compute the face normal using the cross product of two edges of the triangle
        Vector normal = cross(v1 - v0, v2 - v0).normalized();

        // Calculate the dot product between the triangle normal and the ray direction
        float denom = dot(normal, ray.direction);

        // If the ray is nearly parallel to the triangle plane, no hit
        if (std::abs(denom) < 1e-6f) return RT::Trace(false, 0, ray.origin, Point{}, Vector{});

        // Calculate distance t along the ray where it intersects the triangle plane
        float t = dot(normal, v0 - ray.origin) / denom;

        // If intersection is behind the ray origin, no hit
        if (t < 0) return RT::Trace(false, 0, ray.origin, Point{}, Vector{});

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
            // If inside, return a successful hit record with intersection details
            return RT::Trace {
                true,
                t,
                ray.origin,
                p,
                normal
            };
        }

        // Otherwise, no hit
        return RT::Trace(false, 0, ray.origin, Point{}, Vector{});
    }

    RT::Trace Mesh::hit(const Ray& ray) const
    {
        bool hit_any = false;                    // Flag to track if any triangle was hit
        float closest_t = std::numeric_limits<float>::max();  // Closest hit distance initialized to max
        Point hit_position {};                   // Stores the closest hit position
        Vector hit_normal {};                    // Stores the normal at closest hit

        // Iterate over all triangles defined by vertex indices
        for (size_t i = 0; i < indices.size(); ++i)
        {
            // Extract vertex indices for the current triangle
            const auto& [i0, i1, i2] = indices[i];
            const Point& v0 = vertices[i0];
            const Point& v1 = vertices[i1];
            const Point& v2 = vertices[i2];

            // Create a Triangle instance with the current vertices
            Triangle triangle(v0, v1, v2);

            // Perform ray-triangle intersection test
            RT::Trace result = triangle.hit(ray);

            // If hit and closer than any previous hit, update closest hit info
            if (result.hit && result.t < closest_t)
            {
                hit_any = true;
                closest_t = result.t;
                hit_position = result.position;
                hit_normal = result.normal;
            }
        }

        // If any hit found, return the closest hit details
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

        // Otherwise, return no-hit record
        return RT::Trace { false, 0, ray.origin, {}, {} };
    }

    Mesh::Mesh(objReader& reader, Vector color) : Hittable(color)  // Call base constructor with color
    {
        this->vertices = reader.getVertices();     // Load mesh vertices from objReader
        const auto& faces = reader.getFaces();     // Load mesh faces (triangles) from objReader

        indices.reserve(faces.size());              // Reserve space for triangle indices
        triangle_normals.reserve(faces.size());    // Reserve space for triangle normals
        vertex_normals.resize(vertices.size(), Vector(0, 0, 0));  // Initialize vertex normals to zero
        std::vector<int> counts(vertices.size(), 0);              // Track how many triangles share each vertex

        // Iterate over each face (triangle) in the mesh
        for (const auto& face : faces) {
            // Store vertex indices for this triangle
            std::array<int, 3> tri = {
                face.verticeIndice[0],
                face.verticeIndice[1],
                face.verticeIndice[2]
            };
            indices.push_back(tri);   // Add triangle indices to mesh

            // Compute normal for this triangle face
            const Point& a = vertices[tri[0]];
            const Point& b = vertices[tri[1]];
            const Point& c = vertices[tri[2]];

            Vector normal = cross(b - a, c - a).normalized();
            triangle_normals.push_back(normal);  // Store the face normal

            // Accumulate this face normal into each vertex normal for smooth shading
            for (int idx : tri) {
                vertex_normals[idx] += normal;   // Sum normals for averaging
                counts[idx]++;                   // Increment count of faces sharing this vertex
            }
        }

        // Normalize the accumulated vertex normals by averaging
        for (size_t i = 0; i < vertex_normals.size(); ++i) {
            if (counts[i] > 0) {
                vertex_normals[i] = (vertex_normals[i] / float(counts[i])).normalized();
            }
        }
    }

std::shared_ptr<Mesh> transformMesh(const Mesh& original, const Matrix& transform) {
    auto new_mesh = std::make_shared<Geometry::Mesh>(original);  

    for (size_t i = 0; i < new_mesh->vertices.size(); ++i) {
        new_mesh->vertices[i] = transform.applyToPoint(original.vertices[i]);
    }

    for (size_t i = 0; i < new_mesh->vertex_normals.size(); ++i) {
        new_mesh->vertex_normals[i] = transform.applyToVector(original.vertex_normals[i]).normalized();
    }

    return new_mesh;
} 
}