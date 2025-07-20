#include "phong.h"
#include <vector>
#include <iostream>
#include <memory>
#include <algorithm>
#include <cmath>

Vector phongIllumination(
    const RT::Trace& trace,
    const Ray& ray,
    const SceneLights& lights,
    const std::vector<std::shared_ptr<Hittable>>& objects,
    const Material& material
) {
    // Posição, normal e direção da câmera no ponto de interseção, usadas no modelo de iluminação
    const Point& point = trace.position;
    const Vector& normal = trace.normal;
    const Vector viewDir = -ray.direction.normalized();

    // Inicia a cor zerada
    Vector color(0.0);

    // Contribuição da luz ambiente
    color += material.ka * lights.ambient_color;

    for (const auto& light : lights.lights) {
        // Direção da luz normalizada
        Vector lightDir = (light.position - point).normalized();

        // Componente difusa
        double diff = std::max(0.0, dot(normal, lightDir));

        // Direção refletida 
        Vector reflectDir = (-lightDir) - 2.0 * dot(-lightDir, normal) * normal;

        // Componente especular
        double spec = std::pow(std::max(0.0, dot(viewDir, reflectDir)), material.shininess);

        // Distância da luz ao ponto
        double distance = (light.position - point).norm();

        // Raio sombra (0.001 para evitar quando o raio de sombra colide com a própria superfície de onde foi emitido - "acne de sombra")
        Ray shadowRay(point + normal * 0.001, lightDir);

        // Para cada objeto na cena
        bool inShadow = false;
        for (const auto& obj : objects) {
            // Testa se o raio que vai do ponto até a luz (shadowRay) intercepta algum objeto
            RT::Trace shadowTrace = obj->hit(shadowRay);

            // Se o raio colidiu com o objeto e essa colisão está mais perto do que a distância até a luz
            if (shadowTrace.hit && shadowTrace.t < distance) {

                // Então o ponto está em sombra para essa luz
                inShadow = true;

                // Como já sabemos que está em sombra, não precisamos testar os outros objetos
                break;
            }
        }

        if (!inShadow) {
            // Soma contribuição difusa e especular com atenuação
            Vector diffuse = material.kd * light.intensity * diff;
            Vector specular = material.ks * light.intensity * spec;
            color += (diffuse + specular);
        }
    }

    return color;
}

