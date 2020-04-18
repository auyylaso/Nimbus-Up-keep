#pragma once

#include "../SDK/IClientEntity.h" // cbaseplayer
#include <unordered_map>

namespace BoneMaps
{
bool BuildAllBonemaps();
const std::unordered_map<int, int> *GetModelTypeBoneMap(C_BasePlayer *player);
} // namespace BoneMaps