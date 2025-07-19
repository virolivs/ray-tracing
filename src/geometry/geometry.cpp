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
            return RT::Trace { hit, 0.0f, origin, position, normal, this };
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
            return RT::Trace { hit, 0.0f, origin, position, normal, this };
        }

        hit = true;
        position = ray.at(t);
        normal = (position - center).normalized();

        return RT::Trace { hit, t, origin, position, normal, this };

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
            return RT::Trace { hit, 0.0f, origin, position, normal, this };
        }

        double t = dot(n, p - o) / dot(n, d);

        if (t < 0.0f)
        {
            return RT::Trace { hit, 0.0f, origin, position, normal, this };
        }

        hit = true;
        position = ray.at(t);
        normal = n.normalized();

        return RT::Trace { hit, t, origin, position, normal, this };
    }

    RT::Trace Triangle::hit(const Ray& ray) const
    {
        Vector normal = cross(v1 - v0, v2 - v0).normalized();
        double denom = dot(normal, ray.direction);

        double epsilon = 1e-6;

        if (std::abs(denom) <= epsilon)
            return RT::Trace(false, 0, ray.origin, Point{}, Vector{}, this);

        double t = dot(normal, v0 - ray.origin) / denom;

        if (t < 0)
            return RT::Trace(false, 0, ray.origin, Point{}, Vector{}, this);

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
                    this
                };
            }
        }

        return RT::Trace(false, 0, ray.origin, Point{}, Vector{}, this);
    }


    RT::Trace Mesh::hit(const Ray& ray) const
    {
        bool hit_any = false;
        double closest_t = std::numeric_limits<double>::max();
        Point hit_position {};
        Vector hit_normal {};
        size_t hit_index = 0;

        for (size_t i = 0; i < indices.size(); ++i)
        {
            const auto& tri = indices[i];
            const Point& a = vertices[tri[0]];
            const Point& b = vertices[tri[1]];
            const Point& c = vertices[tri[2]];

            Triangle temp(a, b, c, this->material);
            RT::Trace result = temp.hit(ray);

            if (result.hit && result.t < closest_t)
            {
                hit_any = true;
                closest_t = result.t;
                hit_position = result.position;
                hit_normal = result.normal;
                hit_index = i;
            }
        }

        if (hit_any)
        {
            return RT::Trace{ true, closest_t, ray.origin, hit_position, hit_normal, this };
        }

        return RT::Trace{ false, 0, ray.origin, {}, {}, this };
    }

    Mesh::Mesh(objReader& reader) : Hittable()
    {
        // Carrega os vértices, faces e normais a partir do reader
        this->vertices = reader.getVertices();
        const auto& faces = reader.getFaces();
        const auto& normals = reader.getNormals();

        // Reserva espaço para as estruturas de dados baseadas na quantidade de faces
        indices.reserve(faces.size());
        triangle_normals.reserve(faces.size());
        face_colors.reserve(faces.size());
        materials.reserve(faces.size());

        // Inicializa os vetores de normais por vértice e um contador para média
        vertex_normals.resize(vertices.size(), Vector(0, 0, 0));
        std::vector<int> counts(vertices.size(), 0);

        // Para cada face carregada
        for (const auto& face : faces) {
            // Índices dos vértices e das normais da face
            std::array<int, 3> v_idx = { face.verticeIndice[0], face.verticeIndice[1], face.verticeIndice[2] };
            std::array<int, 3> n_idx = { face.normalIndice[0], face.normalIndice[1], face.normalIndice[2] };

            // Armazena os índices dos vértices da face
            indices.push_back(v_idx);

            Vector face_normal;

            // Verifica se os índices de normais são válidos
            bool valid_normals = n_idx[0] >= 0 && n_idx[1] >= 0 && n_idx[2] >= 0 &&
                                n_idx[0] < normals.size() && n_idx[1] < normals.size() && n_idx[2] < normals.size();

            if (valid_normals) {
                // Se todas as normais forem iguais, usa diretamente
                if (n_idx[0] == n_idx[1] && n_idx[1] == n_idx[2]) {
                    face_normal = normals[n_idx[0]];
                } else {
                    // Caso contrário, tira a média das três normais
                    face_normal = (normals[n_idx[0]] + normals[n_idx[1]] + normals[n_idx[2]]) / 3.0;
                }
            } else {
                // Se não houver normais válidas, calcula a normal geométrica da face (produto vetorial)
                const Point& a = vertices[v_idx[0]];
                const Point& b = vertices[v_idx[1]];
                const Point& c = vertices[v_idx[2]];
                face_normal = cross(b - a, c - a);
            }

            // Normaliza a normal da face e armazena
            triangle_normals.push_back(face_normal.normalized());

            // Define uma cor base difusa para a face (pode ser usada para visualização)
            face_colors.push_back(face.kd);

            // Cria o material completo da face com os atributos carregados do .mtl
            Material mat(
                face.kd,             // kd - componente difusa
                face.ks,             // ks - componente especular
                face.ka,             // ka - componente ambiente
                Vector(0, 0, 0),     // kr - reflexão (pode adaptar para face.ke, se necessário)
                Vector(0, 0, 0),     // kt - transmissão
                face.ns              // eta - rugosidade ou brilho (do MTL)
            );

            // Armazena o material
            materials.push_back(mat);

            // Se normais forem válidas, acumula para calcular a normal média por vértice
            if (valid_normals) {
                for (int i = 0; i < 3; ++i) {
                    int v = v_idx[i];
                    int n = n_idx[i];
                    vertex_normals[v] += normals[n];
                    counts[v]++;
                }
            }
        }

        // Calcula a normal média para cada vértice (normaliza após somar as contribuições)
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