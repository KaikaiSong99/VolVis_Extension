#pragma once
#include "render/render_config.h"
#include "volume/secondderivative_volume.h"
#include "volume/volume.h"
#include <GL/glew.h> // Include before glfw3
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace ui {

class TransferFunctionSecondDerivativeWidget {
public:
    TransferFunctionSecondDerivativeWidget(const volume::Volume& volume, const volume::SecondDerivativeVolume& secondDerivative);

    void draw();
    void updateRenderConfig(render::RenderConfig& renderConfig);

private:
    float m_intensity, m_maxIntensity;
    float m_radius;
    float m_threshold;
    glm::vec4 m_color;
    glm::vec4 m_color_2nd_deriv;

    int m_interactingPoint;
    GLuint m_histogramImg;
};
}
