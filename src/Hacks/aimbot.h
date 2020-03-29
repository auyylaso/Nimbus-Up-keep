#pragma once

#include "../SDK/IGameEvent.h"
#include "../SDK/IInputSystem.h"
#include <cstdint>
#include <vector>

namespace Aimbot
{

extern bool aimStepInProgress;		 // This is extern, for other parts. (like auto-wall)
extern std::vector<int64_t> friends; // A list of "friends", which the aimbot wouldn't track as target
extern int targetAimbot;			 // The entity number, for the aimbot to lock on.

//Hooks
void CreateMove(CUserCmd *cmd);
void FireGameEvent(IGameEvent *event);
void UpdateValues();

} // namespace Aimbot
