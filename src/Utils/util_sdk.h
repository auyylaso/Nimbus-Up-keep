#pragma once

#include "../SDK/IInputSystem.h"
#include "../SDK/IMaterial.h"
#include <string>

namespace Util
{

char GetButtonString(ButtonCode_t key);
std::string GetButtonName(ButtonCode_t buttonCode);
ButtonCode_t GetButtonCode(std::string buttonName);

IMaterial *CreateMaterial(std::string type, std::string texture, bool ignorez, bool nofog, bool model, bool nocull, bool halflambert);
IMaterial *CreateMaterial(std::string type, std::string texture, bool ignorez, bool nofog, bool model, bool nocull, bool halflambert, bool additive, std::string envmap, bool normalmapalphaenvmapmask, std::string envmaptint, bool envmapfresnel, std::string envmapfresnelminmaxexp, float alpha, bool envmapcontrast, bool selfillum, bool znearer, bool flat);
const char *GetActivityName(int actNum);

bool IsDangerZone(); // TODO: Use interfaces since we are internal.

} // namespace Util