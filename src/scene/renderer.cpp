#include "renderer.h"
#include <fstream>
#include <iostream>
#include <limits>
#include "light.h" 
#include "raytracer/phong.h"
#include "../geometry/geometry.h"

std::vector<std::shared_ptr<Hittable>> scene;

Vector clamp_color(const Vector& color) {
    return Vector(
        clamp(color.x, 0.0, 1.0),
        clamp(color.y, 0.0, 1.0),
        clamp(color.z, 0.0, 1.0)
    );
}

Vector color(const Ray& ray, const SceneLights& lights, int depth = 0) {
    const int MAX_DEPTH = 3;
    if (depth >= MAX_DEPTH)
        return Vector(0.0);

    // ============================================
    // 1. Encontrar o objeto mais próximo atingido pelo raio
    // ============================================
    double closest_t = std::numeric_limits<double>::max();
    RT::Trace closest_hit;
    bool any_hit = false;

    for (const auto& object : scene) {
        RT::Trace hit_result = object->hit(ray);
        if (hit_result.hit && hit_result.t < closest_t) {
            closest_t = hit_result.t;
            closest_hit = hit_result;
            any_hit = true;
        }
    }

    // ============================================
    // 2. Caso o raio não acerte nada, retornar cor do "sky gradient"
    // ============================================
    if (!any_hit) {
        Vector unit_direction = ray.direction.normalized();
        double t = 0.5 * (unit_direction.y + 1.0);
        return Vector(1.0, 1.0, 1.0) * (1.0 - t) + Vector(0.5f, 0.7f, 1.0) * t;
    }

    // ============================================
    // 3. Determinar material do objeto atingido (se for mesh, pegar material da face)
    // ============================================
    Material mat;
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

    // ============================================
    // 4. Calcular cor local usando modelo de iluminação Phong
    // ============================================
    Vector localColor = phongIllumination(closest_hit, ray, lights, scene, mat);
    Vector finalColor = Vector(0.0);

    // ============================================
    // 5. Calcular cor da reflexão recursivamente
    // ============================================
    Vector reflectedColor(0.0);

    // Cálculo da direção refletida usando a fórmula da reflexão: R = D - 2(D·N)N
    Vector reflectDir = ray.direction - 2.0 * dot(ray.direction, closest_hit.normal) * closest_hit.normal;

    // Para evitar problemas de precisão (artefatos), aplicamos um deslocamento mínimo (bias)
    // Isso impede que o raio refletido "recolida" no mesmo ponto imediatamente
    double bias = 0.001;
    Point reflect_origin = closest_hit.position + bias * closest_hit.normal * (dot(ray.direction, closest_hit.normal) < 0 ? 1.0 : -1.0);

    // Criamos o novo raio refletido e chamamos a função `color` recursivamente
    Ray reflectedRay(reflect_origin, reflectDir.normalized());
    reflectedColor = color(reflectedRay, lights, depth + 1);

    // ============================================
    // 6. Calcular cor da refração recursivamente (se material for parcialmente transparente)
    // ============================================
    Vector refractedColor(0.0);

    // Se a opacidade for menor que 1, significa que o material é parcialmente transparente
    if (mat.opacity < 1.0) {

        // Direção do raio incidente
        Vector D = ray.direction.normalized();

        // Normal da superfície no ponto de interseção
        Vector N = closest_hit.normal.normalized();

        // Índice de refração de entrada (ar = 1.0) e saída (material)
        double ior_in = 1.0;
        double ior_out = mat.ior;

        // Testa se o raio está entrando ou saindo do material
        bool entering = dot(D, N) < 0;

        if (!entering) {
            // Se está saindo, inverte os índices e a normal
            std::swap(ior_in, ior_out);
            N = -N;
        }

        // Relação entre os índices de refração
        double eta = ior_in / ior_out;

        // Cálculo de cosθi
        double cosi = -dot(D, N);

        // Testa se há reflexão total interna
        double k = 1.0 - eta * eta * (1.0 - cosi * cosi);

        if (k >= 0) {
            // Se não houver reflexão total interna, calcula a direção do raio refratado
            Vector refractedDir = eta * D + (eta * cosi - std::sqrt(k)) * N;

            // Desloca a origem do raio para evitar problemas de precisão (bias)
            Point refract_origin = closest_hit.position + bias * refractedDir;

            // Cria o raio refratado e chama recursivamente a função color
            Ray refractedRay(refract_origin, refractedDir.normalized());
            refractedColor = color(refractedRay, lights, depth + 1);
        } else {
            // Se houver reflexão total interna, considera apenas a reflexão
            refractedColor = reflectedColor;
        }
    }


    // ============================================
    // 7. Combinar cor local, reflexão e refração ponderadamente para cor final
    // ============================================

    // Calcula o peso da reflexão com base na intensidade do componente especular (ks)
    // Quanto maior ks, mais reflexivo o material
    double reflectWeight = std::min(1.0, (mat.ks.x + mat.ks.y + mat.ks.z) / 3.0);

    // Combina a cor local com a cor refratada de acordo com a opacidade
    Vector transmitColor = (1.0 - mat.opacity) * refractedColor + mat.opacity * localColor;

    // Combina a cor transmitida com a cor refletida de acordo com o peso da reflexão
    finalColor = (1.0 - reflectWeight) * transmitColor + reflectWeight * reflectedColor;

    // Garante que os valores da cor estejam no intervalo [0,1]
    finalColor = clamp_color(finalColor);

    return finalColor;
}


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

    for (int j = image_height - 1; j >= 0; --j)
    {
        for (int i = 0; i < image_width; ++i)
        {
            Ray ray = camera.cast_ray(i, j);
            Vector pixel_color = color(ray, lights, 0);  // chamada recursiva com profundidade 0
            
            int red   = static_cast<int>(255.99 * clamp(pixel_color.x, 0.0, 1.0));
            int green = static_cast<int>(255.99 * clamp(pixel_color.y, 0.0, 1.0));
            int blue  = static_cast<int>(255.99 * clamp(pixel_color.z, 0.0, 1.0));

            image << red << " " << green << " " << blue << "\n";
        }
    }

    image.close();
    std::cout << "Image saved to " << filename << "\n";
}
