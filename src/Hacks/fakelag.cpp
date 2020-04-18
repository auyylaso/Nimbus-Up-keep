#include "fakelag.h"

#include "../Hooks/hooks.h"
#include "../interfaces.h"
#include "../settings.h"

bool FakeLag::lagSpike = false;

static int ticks = 0;
int ticksMax = 16;

void FakeLag::CreateMove(CUserCmd *cmd)
{
	if (!Settings::FakeLag::enabled)
		return;

	if (Settings::FakeDuck::enabled && inputSystem->IsButtonDown(Settings::FakeDuck::key))
		return;

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->GetAlive())
		return;

	if (/* (!Settings::FakeLag::States::enabled || Settings::FakeLag::States::standValue == 0) && */ localplayer->GetVelocity().Length() < 0.1f)
		return;

	/*
	if (Settings::FakeLag::States::airValue == 0 && !(localplayer->GetFlags() & FL_ONGROUND))
		return;

	if (Settings::FakeLag::States::moveValue == 0 && localplayer->GetVelocity().Length() > 0.0f)
		return;
	*/

	if (cmd->buttons & IN_ATTACK)
	{
		CreateMove::sendPacket = true;
		return;
	}

	if (ticks >= ticksMax)
	{
		CreateMove::sendPacket = true;
		ticks = 0;
	}
	else if (FakeLag::lagSpike)
		CreateMove::sendPacket = false;
	/*
	else if (Settings::FakeLag::States::enabled)
	{
		if (!(localplayer->GetFlags() & FL_ONGROUND))
			CreateMove::sendPacket = ticks < 16 - Settings::FakeLag::States::airValue;
		else if (localplayer->GetVelocity().Length() > 0.1f)
			CreateMove::sendPacket = ticks < 16 - Settings::FakeLag::States::moveValue;
		else
			CreateMove::sendPacket = ticks < 16 - Settings::FakeLag::States::standValue;
	}
	*/
	else
		CreateMove::sendPacket = ticks < 16 - Settings::FakeLag::value;

	ticks++;
}
