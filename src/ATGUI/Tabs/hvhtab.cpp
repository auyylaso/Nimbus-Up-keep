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

            ImGui::Combo(XORSTR("Mode"), (int*)& Settings::AntiAim::type, aTypes, IM_ARRAYSIZE(aTypes));
            ImGui::Separator();

            ImGui::SliderFloat(XORSTR("Desync Multiplier"), &Settings::AntiAim::yaw, -1, 1, "Desync Multiplier: %0.2f");
            ImGui::Separator();

            if (Settings::AntiAim::type == AntiAimType::CUSTOM) {
                ImGui::SliderFloat(XORSTR("Yaw Angle"), &Settings::AntiAim::yaw, 0, 360, "Yaw Angle: %1.0f");
                ImGui::Separator();
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

            ImGui::Checkbox(XORSTR("Freestand"), &Settings::AntiAim::Freestanding::enabled);
            ImGui::Separator();

            ImGui::Text(XORSTR("Disable"));
            ImGui::Separator();
            ImGui::Checkbox(XORSTR("Holding Knife"), &Settings::AntiAim::AutoDisable::knifeHeld);
            ImGui::Separator();

            ImGui::Text(XORSTR("Fake Movement"));
            ImGui::Separator();
            
            ImGui::Checkbox(XORSTR("Fake Lag"), &Settings::FakeLag::enabled);
            if (Settings::FakeLag::enabled)
            {
                ImGui::SliderInt(XORSTR("Default Fake-Lag"), &Settings::FakeLag::value, 0, 16, XORSTR("Amount: %0.f"));
                ImGui::Checkbox(XORSTR("Fake Lag on Peek"), &Settings::FakeLag::lagSpike);
                ImGui::Checkbox(XORSTR("Fake Lag on Move States"), &Settings::FakeLag::States::enabled);
            }

            ImGui::Separator();

            if (Settings::FakeLag::States::enabled && Settings::FakeLag::enabled) {

                ImGui::Text(XORSTR("Fake-Lag States"));
                ImGui::Separator();

                ImGui::SliderInt(XORSTR("Fake-Lag on Stand"), &Settings::FakeLag::States::standValue, 0, 16, XORSTR("Amount: %0.f"));
                ImGui::SliderInt(XORSTR("Fake-Lag on Move"), &Settings::FakeLag::States::moveValue, 0, 16, XORSTR("Amount: %0.f"));
                ImGui::SliderInt(XORSTR("Fake-Lag in Air"), &Settings::FakeLag::States::airValue, 0, 16, XORSTR("Amount: %0.f"));
            }

            ImGui::EndChild();
        }
    }
    ImGui::NextColumn();
    {
        ImGui::BeginChild(XORSTR("HVH2"), ImVec2(0, 0), true);
        {
            ImGui::Text(XORSTR("Resolver"));
            ImGui::Checkbox(XORSTR("Resolver (beta)"), &Settings::Resolver::resolveAll);
            ImGui::Separator();
            ImGui::Checkbox(XORSTR("Anti-Aim States"), &Settings::AntiAim::States::enabled);

            if (Settings::AntiAim::States::enabled)
            {
                ImGui::Separator();
                ImGui::Combo(XORSTR("Anti-Aim in Stand"), (int*)& Settings::AntiAim::States::Stand::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Stand::type == AntiAimType::CUSTOM) {
                    ImGui::SliderFloat(XORSTR("##AASTANDANGLE"), &Settings::AntiAim::States::Stand::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();
               
                ImGui::Combo(XORSTR("Anti-Aim in Movement"), (int*)& Settings::AntiAim::States::Run::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Run::type == AntiAimType::CUSTOM) {
                    ImGui::SliderFloat(XORSTR("##AARUNANGLE"), &Settings::AntiAim::States::Run::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();

                ImGui::Combo(XORSTR("Anti-Aim in Air"), (int*)& Settings::AntiAim::States::Air::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Air::type == AntiAimType::CUSTOM) {
                    ImGui::SliderFloat(XORSTR("##AAAIRANGLE"), &Settings::AntiAim::States::Air::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();
            }

            ImGui::Separator();
            ImGui::Checkbox(XORSTR("LBY Breaker"), &Settings::AntiAim::LBYBreaker::enabled);
            ImGui::SliderFloat(XORSTR("##LBYOFFSET"), &Settings::AntiAim::LBYBreaker::offset, 0, 360, "LBY Offset(from fake): %1.0f");
            ImGui::EndChild();
        }
    }
}
