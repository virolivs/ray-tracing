#include "phong.h"

Vector phongIllumination(
    const RT::Trace& trace,
    const SceneLights& lights,
    const std::vector<std::shared_ptr<Hittable>>& objects
) {
    // Extrai informações do ponto de interseção, normal, direção da visão e material da superfície
    const Vector& point = trace.point;
    const Vector& normal = trace.normal;
    const Vector& viewDir = -trace.ray.direction.normalized();
    const Material& material = trace.hittable->material;

    // Inicializa a cor resultante com a contribuição da luz ambiente
    Vector color(0.0f);
    color += material.ka * lights.ambient_color;

    // Para cada luz da cena, calcula a contribuição difusa, especular e a sombra
    for (const auto& light : lights.lights)  {
        Vector lightDir = (light.position - point).normalized();
        float diff = std::max(0.0f, dot(normal, lightDir));
        Vector reflectDir = reflect(-lightDir, normal);
        float spec = std::pow(std::max(dot(viewDir, reflectDir), 0.0f), material.eta);

        // Calcula a atenuação da luz com base na distância
        float distance = (light.position - point).length();
        float attenuation = 1.0f / (distance * distance);

        // Gera um raio para verificar sombras: se há objeto entre a luz e o ponto, este fica sombreado
        Ray shadowRay(point + normal * 0.001f, lightDir);
        bool inShadow = false;
        for (const auto& obj : objects) {
            RT::Trace shadowTrace = obj->hit(shadowRay);
            if (shadowTrace.hit && shadowTrace.t < distance) {
                inShadow = true;
                break;
            }
        }

        // Se não está em sombra, adiciona contribuição de luz ambiente, difusa e especular atenuada
        // Caso contrário, apenas a contribuição ambiente atenuada é adicionada para suavizar o efeito
        if (!inShadow) {
            Vector diffuse = material.kd * light.intensity * diff;
            Vector specular = material.ks * light.intensity * spec;
            color += (diffuse + specular) * attenuation;
        }
    }

    // Retorna a cor final calculada considerando todas as luzes e sombras
    return color;
}
