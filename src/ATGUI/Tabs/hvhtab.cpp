#include "hvhtab.h"

#include "../../interfaces.h"
#include "../../Utils/xorstring.h"
#include "../../settings.h"
#include "../../Hacks/valvedscheck.h"
#include "../../Hacks/antiaim.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"

#pragma GCC diagnostic ignored "-Wformat-security"

void HvH::RenderTab()
{
    const char* aTypes[] = {
            "Rage", "Legit", "Custom"
    };

    ImGui::Columns(2, nullptr, true);
    {
        ImGui::BeginChild(XORSTR("HVH1"), ImVec2(0, 0), true);
        {
            ImGui::Checkbox(XORSTR("Anti-Aim"), &Settings::AntiAim::enabled);
            ImGui::Separator();

            ImGui::Checkbox(XORSTR("State Based Anti-Aim"), &Settings::AntiAim::States::enabled);
            ImGui::Separator();

            if (!Settings::AntiAim::States::enabled) {
                ImGui::Combo(XORSTR("Mode"), (int*)& Settings::AntiAim::type, aTypes, IM_ARRAYSIZE(aTypes));
                ImGui::Separator();
            }
            else
            {
                ImGui::Combo(XORSTR("Stand-type"), (int*)& Settings::AntiAim::States::Stand::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Stand::type == AntiAimType::CUSTOM) {
                    ImGui::Separator();
                    ImGui::SliderFloat(XORSTR("##STANDANGLE"), &Settings::AntiAim::States::Stand::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();

                ImGui::Combo(XORSTR("Walk-type (IN_WALK flag broken)"), (int*)& Settings::AntiAim::States::Walk::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Walk::type == AntiAimType::CUSTOM) {
                    ImGui::Separator();
                    ImGui::SliderFloat(XORSTR("##WALKANGLE"), &Settings::AntiAim::States::Walk::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();

                ImGui::Combo(XORSTR("Run-type"), (int*)& Settings::AntiAim::States::Run::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Run::type == AntiAimType::CUSTOM) {
                    ImGui::Separator();
                    ImGui::SliderFloat(XORSTR("##RUNANGLE"), &Settings::AntiAim::States::Run::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();

                ImGui::Combo(XORSTR("Air-type"), (int*)& Settings::AntiAim::States::Air::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Air::type == AntiAimType::CUSTOM) {
                    ImGui::Separator();
                    ImGui::SliderFloat(XORSTR("##AIRANGLE"), &Settings::AntiAim::States::Air::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();
            }
            

            ImGui::Text(XORSTR("Anti-Aim Keys"));
            ImGui::Separator();

            ImGui::Columns(2, nullptr, true);
            {
                ImGui::Text(XORSTR("Left-key"));
                ImGui::Text(XORSTR("Right-key"));
            }
            ImGui::NextColumn();
            {
                UI::KeyBindButton(&Settings::AntiAim::left);
                UI::KeyBindButton(&Settings::AntiAim::right);
            }
            ImGui::Columns(1);
            ImGui::Separator();

            ImGui::Text(XORSTR("Disable"));
            ImGui::Separator();
            ImGui::Checkbox(XORSTR("Knife"), &Settings::AntiAim::AutoDisable::knifeHeld);
            ImGui::Separator();

            ImGui::Text(XORSTR("Freestanding"));
            ImGui::Separator();

            if (Settings::AntiAim::type == AntiAimType::CUSTOM || !Settings::AntiAim::States::enabled) {
                ImGui::Separator();
                ImGui::SliderFloat(XORSTR("Yaw Angle"), &Settings::AntiAim::yaw, 0, 360, "Yaw angle %1.0f");
            }

            ImGui::EndChild();
        }
    }
    ImGui::NextColumn();
    {
        ImGui::BeginChild(XORSTR("HVH2"), ImVec2(0, 0), true);
        {
            ImGui::Text(XORSTR("Resolver"));
            ImGui::Separator();
            ImGui::Checkbox(XORSTR("Resolve All"), &Settings::Resolver::resolveAll);
            ImGui::Separator();
            ImGui::Text(XORSTR("Movement"));
            ImGui::Checkbox(XORSTR("Auto Crouch"), &Settings::Aimbot::AutoCrouch::enabled);
            ImGui::Separator();
            ImGui::Checkbox(XORSTR("LBY Breaker"), &Settings::AntiAim::LBYBreaker::enabled);
            if (Settings::AntiAim::LBYBreaker::enabled) {
                ImGui::Checkbox(XORSTR("Custom Breaker Angle"), &Settings::AntiAim::LBYBreaker::custom);
            }
            if (Settings::AntiAim::LBYBreaker::enabled && Settings::AntiAim::LBYBreaker::custom) {
                ImGui::SliderFloat(XORSTR("##LBYOFFSET"), &Settings::AntiAim::LBYBreaker::offset, 0, 360, "LBY Offset(from fake): %1.0f");
            }
            ImGui::EndChild();
        }
    }
}
