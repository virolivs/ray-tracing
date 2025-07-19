#include "phong.h"
#include <vector>
#include <memory>
#include <algorithm> // para std::max
#include <cmath>     // para std::pow

Vector phongIllumination(
    const RT::Trace& trace,
    const Ray& ray, // adiciona o Ray original para usar direção do olho
    const SceneLights& lights,
    const std::vector<std::shared_ptr<Hittable>>& objects
) {
    const Point& point = trace.position;          // ponto de interseção
    const Vector& normal = trace.normal;          // normal no ponto
    const Vector viewDir = -ray.direction.normalized();  // direção do olho (raio invertido)

    const Material& material = trace.hittable->material; // material do objeto atingido

    Vector color(0.0);  // cor inicial zero, usando double

    color += material.ka * lights.ambient_color;  // contribuição da luz ambiente

    for (const auto& light : lights.lights) {
        Vector lightDir = (light.position - point).normalized(); // direção da luz

        double diff = std::max(0.0, dot(normal, lightDir));      // componente difusa
        Vector reflectDir = reflect(-lightDir, normal);          // direção refletida
        double spec = std::pow(std::max(0.0, dot(viewDir, reflectDir)), material.eta);  // componente especular

        double distance = (light.position - point).norm();       // distância para atenuação
        double attenuation = 1.0 / (distance * distance);        // atenuação quadrática

        Ray shadowRay(point + normal * 0.001, lightDir);         // raio sombra para evitar acne de sombra
        bool inShadow = false;

        // verifica se o ponto está em sombra para esta luz
        for (const auto& obj : objects) {
            RT::Trace shadowTrace = obj->hit(shadowRay);
            if (shadowTrace.hit && shadowTrace.t < distance) {
                inShadow = true;
                break;
            }
        }

        if (!inShadow) {
            // soma contribuição difusa e especular com atenuação
            Vector diffuse = material.kd * light.intensity * diff;
            Vector specular = material.ks * light.intensity * spec;
            color += (diffuse + specular) * attenuation;
        }
        // se em sombra, só luz ambiente já foi adicionada no começo
    }

    return color;  // retorna cor final
}
