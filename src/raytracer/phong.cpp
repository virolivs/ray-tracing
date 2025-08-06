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
    const std::vector<std::shared_ptr<Hittable>>& objects
) {
    const Point& point = trace.position;
    const Vector& normal = trace.normal;
    const Vector viewDir = -ray.direction.normalized();

    const Material& material = trace.hittable->get_material(trace.face_index);

    Vector color(0.0);

    // Componente ambiente
    color += material.ka * lights.ambient_color;

    for (const auto& light : lights.lights) {
        Vector lightDir = (light.position - point).normalized();

        double diff = std::max(0.0, dot(normal, lightDir));
        Vector reflectDir = (-lightDir) - 2.0 * dot(-lightDir, normal) * normal;
        double spec = std::pow(std::max(0.0, dot(viewDir, reflectDir)), material.shininess);

        double distance = (light.position - point).norm();
        Ray shadowRay(point + normal * 0.001, lightDir);

        bool inShadow = false;
        for (const auto& obj : objects) {
            RT::Trace shadowTrace = obj->hit(shadowRay);
            if (shadowTrace.hit && shadowTrace.t < distance) {
                inShadow = true;
                break;
            }
        }

        if (!inShadow) {
            Vector diffuse = material.kd * light.intensity * diff;
            Vector specular = material.ks * light.intensity * spec;
            color += diffuse + specular;
        }
    }

    // Emissão + opacidade
    color += material.ke;
    color = material.opacity * color;

    return color;
}

