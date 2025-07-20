#include <cmath>
#include "geometry.h"
#include <memory>

    namespace Geometry
    {
        RT::Trace Sphere::hit(const Ray& ray) const
        {
            Vector o = ray.origin - center;
            Vector d = ray.direction;
            double r = radius;

            double a = dot(d, d);
            double b = 2.0 * dot(o, d);
            double c = dot(o, o) - r * r;
            double discriminant = b * b - 4.0 * a * c;

            if (discriminant < 0.0)
            {
                return RT::Trace{ false, 0.0, ray.origin, {}, {}, this, -1 };
            }

            double sqrt_disc = std::sqrt(discriminant);
            double t1 = (-b - sqrt_disc) / (2.0 * a);
            double t2 = (-b + sqrt_disc) / (2.0 * a);

            double t = (t1 > 0.0) ? t1 : ((t2 > 0.0) ? t2 : -1.0);

            if (t < 0.0)
            {
                return RT::Trace{ false, 0.0, ray.origin, {}, {}, this, -1 };
            }

            Point position = ray.at(t);
            Vector normal = (position - center).normalized();

            return RT::Trace{ true, t, ray.origin, position, normal, this, -1 };
        }

        RT::Trace Plane::hit(const Ray& ray) const
        {
            double denom = dot(normal, ray.direction);
            constexpr double epsilon = 1e-6;

            if (std::abs(denom) < epsilon)
            {
                return RT::Trace{ false, 0.0, ray.origin, {}, {}, this, -1 };
            }

            double t = dot(normal, point - ray.origin) / denom;

            if (t < 0.0)
            {
                return RT::Trace{ false, 0.0, ray.origin, {}, {}, this, -1 };
            }

            Point position = ray.at(t);
            return RT::Trace{ true, t, ray.origin, position, normal.normalized(), this, -1 };
        }

        RT::Trace Triangle::hit(const Ray& ray) const
        {
            Vector n = cross(v1 - v0, v2 - v0).normalized();
            double denom = dot(n, ray.direction);

            constexpr double epsilon = 1e-6;
            if (std::abs(denom) <= epsilon)
                return RT::Trace{ false, 0, ray.origin, {}, {}, this, -1 };

            double t = dot(n, v0 - ray.origin) / denom;
            if (t < 0)
                return RT::Trace{ false, 0, ray.origin, {}, {}, this, -1 };

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
            if (D == 0.0)
                return RT::Trace{ false, 0, ray.origin, {}, {}, this, -1 };

            double s = (vv * wu - uv * wv) / D;
            double t_bary = (uu * wv - uv * wu) / D;

            if (s >= 0 && t_bary >= 0 && (s + t_bary) <= 1)
            {
                return RT::Trace{ true, t, ray.origin, p, n, this, -1 };
            }

            return RT::Trace{ false, 0, ray.origin, {}, {}, this, -1 };
        }

        RT::Trace Mesh::hit(const Ray& ray) const
        {
            bool hit_any = false;
            double closest_t = std::numeric_limits<double>::max();
            Point hit_position {};
            Vector hit_normal {};
            int hit_index = -1;

            for (size_t i = 0; i < indices.size(); ++i)
            {
                const auto& tri = indices[i];
                const Point& a = vertices[tri[0]];
                const Point& b = vertices[tri[1]];
                const Point& c = vertices[tri[2]];

                Triangle temp(a, b, c, materials[i]);
                RT::Trace result = temp.hit(ray);

                if (result.hit && result.t < closest_t)
                {
                    hit_any = true;
                    closest_t = result.t;
                    hit_position = result.position;
                    hit_normal = result.normal;
                    hit_index = static_cast<int>(i);
                }
            }

            if (hit_any)
            {
                return RT::Trace{ true, closest_t, ray.origin, hit_position, hit_normal.normalized(), this, hit_index };
            }

            return RT::Trace{ false, 0, ray.origin, {}, {}, this, -1 };
        }


    Mesh::Mesh(objReader& reader) : Hittable()
    {
        this->vertices = reader.getVertices();
        const auto& faces = reader.getFaces();
        const auto& normals = reader.getNormals();

        indices.reserve(faces.size());
        triangle_normals.reserve(faces.size());
        face_colors.reserve(faces.size());
        materials.reserve(faces.size());

        vertex_normals.resize(vertices.size(), Vector(0, 0, 0));
        std::vector<int> counts(vertices.size(), 0);

        for (const auto& face : faces) {
            std::array<int, 3> v_idx = { face.verticeIndice[0], face.verticeIndice[1], face.verticeIndice[2] };
            std::array<int, 3> n_idx = { face.normalIndice[0], face.normalIndice[1], face.normalIndice[2] };

            indices.push_back(v_idx);

            Vector face_normal;

            bool valid_normals = n_idx[0] >= 0 && n_idx[1] >= 0 && n_idx[2] >= 0 &&
                                n_idx[0] < normals.size() && n_idx[1] < normals.size() && n_idx[2] < normals.size();

            if (valid_normals) {
                if (n_idx[0] == n_idx[1] && n_idx[1] == n_idx[2]) {
                    face_normal = normals[n_idx[0]];
                } else {
                    face_normal = (normals[n_idx[0]] + normals[n_idx[1]] + normals[n_idx[2]]) / 3.0;
                }
            } else {
                const Point& a = vertices[v_idx[0]];
                const Point& b = vertices[v_idx[1]];
                const Point& c = vertices[v_idx[2]];
                face_normal = cross(b - a, c - a);
            }

            triangle_normals.push_back(face_normal.normalized());

            face_colors.push_back(face.kd);

            Material mat(
                face.ka,
                face.kd,
                face.ks,
                face.ke,
                face.ns,
                face.ni,
                face.d
            );

            materials.push_back(mat);

            if (valid_normals) {
                for (int i = 0; i < 3; ++i) {
                    int v = v_idx[i];
                    int n = n_idx[i];
                    vertex_normals[v] += normals[n];
                    counts[v]++;
                }
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

        double det = determinant(transform);
        if (det < 0) {
            for (auto& tri : new_mesh->indices) {
                std::swap(tri[1], tri[2]);
            }
        }

        return new_mesh;
    }

}
