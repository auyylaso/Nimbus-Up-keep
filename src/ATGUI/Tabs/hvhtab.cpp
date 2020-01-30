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

            if (Settings::AntiAim::type == AntiAimType::CUSTOM) {
                ImGui::SliderFloat(XORSTR("Yaw Angle"), &Settings::AntiAim::yaw, 0, 360, "Yaw angle %1.0f");
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

            ImGui::Text(XORSTR("Fake Movement"));
            ImGui::Separator();
            
            ImGui::Columns(2, nullptr, true);
            {
                ImGui::Checkbox(XORSTR("Fake Lag"), &Settings::FakeLag::enabled);
                if (Settings::FakeLag::enabled)
                {
                    ImGui::Checkbox(XORSTR("Fake Lag States"), &Settings::FakeLag::States::enabled);
                    ImGui::Checkbox(XORSTR("Fake Lag On Peek"), &Settings::FakeLag::lagSpike);
                }
            }
            ImGui::NextColumn();
            {
                if (Settings::FakeLag::enabled)
                {
                    ImGui::PushItemWidth(-1);
                    ImGui::SliderInt(XORSTR("##FAKELAGAMOUNT"), &Settings::FakeLag::value, 0, 16, XORSTR("Amount: %0.f"));
                    ImGui::PopItemWidth();
                }
            }
            ImGui::Separator();

            if (Settings::FakeLag::States::enabled) {
                ImGui::Columns(2, nullptr, true);
                {
                    ImGui::Checkbox(XORSTR("Stand Fake Lag"), &Settings::FakeLag::States::Standing::enabled);
                    ImGui::Checkbox(XORSTR("Move Fake Lag"), &Settings::FakeLag::States::Moving::enabled);
                    ImGui::Checkbox(XORSTR("In-air Fake Lag"), &Settings::FakeLag::States::Air::enabled);
                }
                ImGui::NextColumn();
                {
                    ImGui::PushItemWidth(-1);
                    ImGui::SliderInt(XORSTR("##STANDFAKELAGAMOUNT"), &Settings::FakeLag::States::Standing::value, 0, 16, XORSTR("Amount: %0.f"));
                    ImGui::PopItemWidth();
                    ImGui::PushItemWidth(-1);
                    ImGui::SliderInt(XORSTR("##MOVEFAKELAGAMOUNT"), &Settings::FakeLag::States::Moving::value, 0, 16, XORSTR("Amount: %0.f"));
                    ImGui::PopItemWidth();
                    ImGui::PushItemWidth(-1);
                    ImGui::SliderInt(XORSTR("##INAIRFAKELAGAMOUNT"), &Settings::FakeLag::States::Air::value, 0, 16, XORSTR("Amount: %0.f"));
                    ImGui::PopItemWidth();
                }
            }

            ImGui::EndChild();
        }
    }
    ImGui::NextColumn();
    {
        ImGui::BeginChild(XORSTR("HVH2"), ImVec2(0, 0), true);
        {
            ImGui::Separator();
            ImGui::Checkbox(XORSTR("Anti-Aim States"), &Settings::AntiAim::States::enabled);

            if (Settings::AntiAim::States::enabled)
            {
                ImGui::Separator();
                ImGui::Text(XORSTR("Stand Anti-Aim"));
                ImGui::Combo(XORSTR("##AASTAND"), (int*)& Settings::AntiAim::States::Stand::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Stand::type == AntiAimType::CUSTOM) {
                    ImGui::SliderFloat(XORSTR("##AASTANDANGLE"), &Settings::AntiAim::States::Stand::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();

                /* Removed due to broken flag will fix later.
                ImGui::Combo(XORSTR("Walk Type (IN_WALK flag broken)"), (int*)& Settings::AntiAim::States::Walk::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Walk::type == AntiAimType::CUSTOM) {
                    ImGui::SliderFloat(XORSTR("##AAWALKANGLE"), &Settings::AntiAim::States::Walk::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();
                */
               
                ImGui::Text(XORSTR("Move Anti-Aim"));
                ImGui::Combo(XORSTR("##AAMOVE"), (int*)& Settings::AntiAim::States::Run::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Run::type == AntiAimType::CUSTOM) {
                    ImGui::SliderFloat(XORSTR("##AARUNANGLE"), &Settings::AntiAim::States::Run::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();

                ImGui::Text(XORSTR("Air Anti-Aim"));
                ImGui::Combo(XORSTR("##AAAIR"), (int*)& Settings::AntiAim::States::Air::type, aTypes, IM_ARRAYSIZE(aTypes));
                if (Settings::AntiAim::States::Air::type == AntiAimType::CUSTOM) {
                    ImGui::SliderFloat(XORSTR("##AAAIRANGLE"), &Settings::AntiAim::States::Air::angle, 0, 360, "Yaw angle: %1.0f");
                } ImGui::Separator();
            }

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
