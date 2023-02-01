#pragma once
#include "render/render_config.h"
#include "volume/secondderivative_volume.h"
#include "volume/volume.h"
#include <GL/glew.h> // Include before glfw3
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace ui {

class GoochWidget {
public:
    GoochWidget();

    void draw();
    void updateRenderConfig(render::RenderConfig& renderConfig);

private:
    glm::vec3 m_colorWarm;
    glm::vec3 m_colorCold;

    int m_interactingPoint;
};
}
