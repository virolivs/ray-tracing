#include "phong.h"
#include <vector>
#include <iostream>
#include <memory>
#include <algorithm> // para std::max
#include <cmath>     // para std::pow

Vector phongIllumination(
    const RT::Trace& trace,
    const Ray& ray,
    const SceneLights& lights,
    const std::vector<std::shared_ptr<Hittable>>& objects,
    const Material& material  // já vem aqui, não declare de novo
) {
    const Point& point = trace.position;
    const Vector& normal = trace.normal;
    const Vector viewDir = -ray.direction.normalized();

    //const Material& material = trace.hittable->material; // material do objeto atingido

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

        // Para cada objeto na cena
        for (const auto& obj : objects) {

            // Testa se o raio que vai do ponto até a luz (shadowRay) intercepta o objeto
            RT::Trace shadowTrace = obj->hit(shadowRay);

            // Se o raio colidiu com o objeto (hit == true)
            // E essa colisão está mais perto do que a distância até a luz (shadowTrace.t < distance)
            if (shadowTrace.hit && shadowTrace.t < distance) {

                // Então o ponto está em sombra para essa luz
                inShadow = true;

                // Como já sabemos que está em sombra, não precisamos testar os outros objetos
                break;
            }
        }

        if (!inShadow) {
            // soma contribuição difusa e especular com atenuação
            Vector diffuse = material.kd * light.intensity * diff;
            Vector specular = material.ks * light.intensity * spec;
            color += (diffuse + specular); // * attenuation;
        }
        // se em sombra, só luz ambiente já foi adicionada no começo
    }

    //std::cout << "ka: " << material.ka << ", kd: " << material.kd << ", ks: " << material.ks << "\n";
    //std::cout << "Returning color " << color << "\n";


    return color;  // retorna cor final
}

