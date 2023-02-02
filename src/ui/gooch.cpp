#include "gooch.h"
#include <algorithm>
#include <array>
#include <filesystem>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>
#include <iostream>
#include <vector>

static ImVec2 glmToIm(const glm::vec2& v);
static glm::vec2 ImToGlm(const ImVec2& v);
static std::vector<glm::vec4> createHistogramImage(
    const volume::Volume& volume, const volume::SecondDerivativeVolume& secondDerivative, const glm::ivec2& res);

namespace ui {

static constexpr glm::ivec2 widgetSize { 475, 300 };

GoochWidget::GoochWidget()
    : m_colorWarm(0.9f, 0.3f, 0.3f)
    , m_colorCold(0.0f, 0.0f, 1.0f)
    , m_interactingPoint(-1)
{
}

// Draw the widget and handle interactions
void GoochWidget::draw()
{
    const ImGuiIO& io = ImGui::GetIO();

    ImGui::Text("Gooch shading color picker");
    ImGui::Text("You can choose warm color on the left and cold color on the right");

    const glm::vec2 canvasSize { widgetSize.x, widgetSize.y - 20 };
    glm::vec2 canvasPos = ImToGlm(ImGui::GetCursorScreenPos()); // this is the imgui draw cursor, not mouse cursor
    const float xOffset = (ImToGlm(ImGui::GetContentRegionAvail()).x - canvasSize.x);
    canvasPos.x += xOffset; // center widget

    // Detect and handle mouse interaction.
    if (!io.MouseDown[0] && !io.MouseDown[1]) {
        m_interactingPoint = -1;
    }

    // Draw color picker.
    float menuWidth = ImGui::GetContentRegionAvailWidth();
    ImGui::NewLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset / 2);
    ImGui::PushItemWidth(menuWidth * 0.2f);
    ImGui::ColorPicker4("Color1", glm::value_ptr(m_colorWarm));
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset / 2);
    ImGui::PushItemWidth(menuWidth * 0.2f);
    ImGui::ColorPicker4("Color2", glm::value_ptr(m_colorCold));
}

void GoochWidget::updateRenderConfig(render::RenderConfig& renderConfig)
{
    renderConfig.GoochWarmColor = m_colorWarm;
    renderConfig.GoochColdColor = m_colorCold;
}
}

static ImVec2 glmToIm(const glm::vec2& v)
{
    return ImVec2(v.x, v.y);
}

static glm::vec2 ImToGlm(const ImVec2& v)
{
    return glm::vec2(v.x, v.y);
}
