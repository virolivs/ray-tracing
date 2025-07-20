#pragma once

#include <vector>
#include "../lib/point.h"
#include "../lib/vector.h"

// Representa uma fonte de luz pontual
struct Light {
    Point position {};      // Posição da luz no espaço 3D
    Vector intensity {};    // Intensidade (ou cor) da luz

    // Construtor da luz com posição e intensidade
    Light(const Point& pos, const Vector& intensity)
        : position(pos), intensity(intensity) {}
};

// Representa todas as luzes da cena, incluindo luz ambiente
struct SceneLights {
    std::vector<Light> lights;  // Lista de luzes pontuais na cena
    Vector ambient_color {};    // Cor da luz ambiente

    SceneLights(const Vector& ambient = Vector(0, 0, 0))
        : ambient_color(ambient) {}
};
