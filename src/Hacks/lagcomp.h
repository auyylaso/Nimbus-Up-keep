#pragma once

#include "../SDK/CBaseClientState.h"
#include "../SDK/IInputSystem.h"
#include "../Utils/entity.h"

#include <vector>

#define TIME_TO_TICKS(dt) ((int)(0.5f + (float)(dt) / globalVars->interval_per_tick))
#define TICKS_TO_TIME(t) (globalVars->interval_per_tick * (t))

namespace LagComp
{

struct LagCompRecord
{
	C_BasePlayer *entity;
	Vector head, origin;
	matrix3x4_t bone_matrix[128];
};

// stores information about all players for one tick
struct LagCompTickInfo
{
	int tickCount;
	float simulationTime;
	std::vector<LagCompRecord> records;
};

void CreateMove(CUserCmd *cmd);

extern std::vector<LagComp::LagCompTickInfo> lagCompTicks;

} // namespace LagComp
