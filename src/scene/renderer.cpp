#include "renderer.h"
#include <fstream>
#include <iostream>
#include <limits>
#include "light.h" 
#include "raytracer/phong.h"
#include "../geometry/geometry.h"

// Vetor global que armazena todos os objetos da cena (esferas, malhas, etc)
std::vector<std::shared_ptr<Hittable>> scene;

// Função que calcula a cor vista por um raio na cena considerando as luzes
Vector color(const Ray& ray, const SceneLights& lights) {
    double closest_t = std::numeric_limits<double>::max();
    RT::Trace closest_hit;
    bool any_hit = false;

    // Percorre todos os objetos da cena para encontrar o impacto mais próximo
    for (const auto& object : scene) {
        RT::Trace hit_result = object->hit(ray);
        if (hit_result.hit && hit_result.t < closest_t) {
            closest_t = hit_result.t;
            closest_hit = hit_result;
            any_hit = true;
        }
    }

    // Se não houve impacto, retorna cor do "céu" — gradiente azul claro para cima, branco para baixo
    if (!any_hit) {
        Vector unit_direction = ray.direction.normalized();
        double t = 0.5f * (unit_direction.y + 1.0f);
        return Vector(1.0f, 1.0f, 1.0f) * (1.0f - t) + Vector(0.5f, 0.7f, 1.0f) * t;
    }

    // Caso tenha havido impacto, precisamos determinar o material correto para calcular iluminação
    Material mat;

    // Se o objeto atingido for uma Mesh, pegamos o material da face que foi atingida
    if (auto mesh = dynamic_cast<const Geometry::Mesh*>(closest_hit.hittable)) {
        int idx = closest_hit.face_index;
        if (idx >= 0 && idx < (int)mesh->materials.size()) {
            mat = mesh->materials[idx];
        } else {
            mat = closest_hit.hittable->material;
        }
    } else {
        mat = closest_hit.hittable->material;
    }

    // Chama função de iluminação Phong adaptada para receber o material
    return phongIllumination(closest_hit, ray, lights, scene, mat);
}

// Função que gera a imagem da cena renderizada a partir da câmera
void render_scene(const Camera& camera,
                  const std::string& filename,
                  uint32_t image_width,
                  uint32_t image_height,
                  const SceneLights& lights)
{
    std::ofstream image(filename);
    if (!image) {
        std::cerr << "Error creating " << filename << "\n";
        return;
    }

    image << "P3\n" << image_width << " " << image_height << "\n255\n";

    // Loop para cada pixel da imagem, varrendo linha por linha de cima para baixo
    for (int j = image_height - 1; j >= 0; --j)
    {
        for (int i = 0; i < image_width; ++i)
        {
            Ray ray = camera.cast_ray(i, j);
            Vector pixel_color = color(ray, lights);
            
            int red   = static_cast<int>(255.99 * clamp(pixel_color.x, 0.0, 1.0));
            int green = static_cast<int>(255.99 * clamp(pixel_color.y, 0.0, 1.0));
            int blue  = static_cast<int>(255.99 * clamp(pixel_color.z, 0.0, 1.0));

            image << red << " " << green << " " << blue << "\n";
        }
    }

    image.close();
    std::cout << "Image saved to " << filename << "\n";
}
