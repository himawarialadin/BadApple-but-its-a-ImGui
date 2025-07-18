# define IMGUI_DEFINE_MATH_OPERATORS
# include "widgets.h"
# include <imgui_internal.h>

void ax::Widgets::Icon(const ImVec2& size, IconType type, bool filled, const ImVec4& color/* = ImVec4(1, 1, 1, 1)*/, const ImVec4& innerColor/* = ImVec4(0, 0, 0, 0)*/)
{
    if (ImGui::IsRectVisible(size))
    {
        auto cursorPos = ImGui::GetCursorScreenPos();
        auto drawList  = ImGui::GetWindowDrawList();
        ax::Drawing::DrawIcon(drawList, cursorPos, cursorPos + size, type, filled, ImColor(color), ImColor(innerColor));
    
    /*    auto debug = ImGui::GetWindowDrawList();
        debug->AddRectFilled(cursorPos, cursorPos + size, ImColor(ImVec4(0, 1, 0, 1)));

        auto Red = ImColor(ImVec4(1, 0, 0, 1));
        auto Green = ImColor(ImVec4(0, 1, 0, 1));
        auto Blue = ImColor(ImVec4(0, 0, 1, 1));
        auto White = ImColor(ImVec4(1, 1, 1, 1));

        debug->AddCircleFilled(cursorPos,                                   1.0f, Red);
        debug->AddCircleFilled(cursorPos + ImVec2{size.x, 0},    1.0f, Green);
        debug->AddCircleFilled(cursorPos + size,                            1.0f, Blue);
        debug->AddCircleFilled(cursorPos + ImVec2{0, size.y},   1.0f, White);*/


        //ImGui::SetNextWindowPos(cursorPos);
        //ImGui::Begin("This");
        //ImGui::End();
        
    
    }

    ImGui::Dummy(size);
}

