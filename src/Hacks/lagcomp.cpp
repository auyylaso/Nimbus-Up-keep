#include "lagcomp.h"

#include "../Utils/math.h"
#include "../interfaces.h"
#include "../settings.h"

bool Settings::LagComp::enabled = false;

std::vector<LagComp::LagCompFrameInfo> LagComp::lagCompFrames;

void RemoveInvalidTicks()
{
	while (LagComp::lagCompFrames.size() > 24)
		LagComp::lagCompFrames.pop_back();
}

void RegisterTicks()
{
	const auto curframe = LagComp::lagCompFrames.insert(LagComp::lagCompFrames.begin(), { globalVars->tickcount, globalVars->curtime });
	const auto localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());

	for (int i = 1; i < engine->GetMaxClients(); ++i)
	{
		const auto player = (C_BasePlayer*)entityList->GetClientEntity(i);

		if (!player
			|| player == localplayer
			|| player->GetDormant()
			|| !player->GetAlive()
			|| Entity::IsTeamMate(player, localplayer)
			|| player->GetImmune())
			continue;

		LagComp::LagCompRecord record;

		record.entity = player;
		record.origin = player->GetVecOrigin();
		record.head = player->GetBonePosition(BONE_HEAD);

		if (player->SetupBones(record.bone_matrix, 128, BONE_USED_BY_HITBOX, globalVars->curtime))
			curframe->records.push_back(record);
	}
}

void LagComp::CreateMove(CUserCmd* cmd)
{
	if (!Settings::LagComp::enabled)
		return;

	RemoveInvalidTicks();
	RegisterTicks();

	C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->GetAlive())
		return;

	float serverTime = localplayer->GetTickBase() * globalVars->interval_per_tick;
	const auto weapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());

	QAngle my_angle;
	engine->GetViewAngles(my_angle);
	QAngle my_angle_rcs = my_angle + *localplayer->GetAimPunchAngle();

	if (cmd->buttons & IN_ATTACK && weapon->GetNextPrimaryAttack() <= serverTime)
	{
		float fov = Settings::Aimbot::AutoAim::fov * 2;

		int tickcount = 0;
		bool has_target = false;

		for (auto&& frame : LagComp::lagCompFrames)
		{
			for (auto& record : frame.records)
			{
				float tmpFOV = Math::GetFov(
					my_angle_rcs,
					Math::CalcAngle(localplayer->GetEyePosition(), record.head));

				if (tmpFOV < fov)
				{
					fov = tmpFOV;
					tickcount = frame.tickCount;
					has_target = true;
				}
			}
		}

		if (has_target)
		{
			cmd->tick_count = tickcount;
		}
	}
}
