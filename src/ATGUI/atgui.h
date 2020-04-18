#pragma once

#include "../ImGUI/imgui.h"
#include "../SDK/IInputSystem.h"

namespace UI
{
extern bool isVisible;

void SetVisible(bool visible);
void DrawImWatermark();
void SetupColors();
void SetupWindows();
bool ColorPicker(float *col, bool alphabar);
bool ColorPicker3(float col[3]);
bool ColorPicker4(float col[4]);
void KeyBindButton(ButtonCode_t *key);
void UpdateWeaponSettings();
void ReloadWeaponSettings();
} // namespace UI

inline void SetTooltip(const char *text)
{
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("%s", text);
}
