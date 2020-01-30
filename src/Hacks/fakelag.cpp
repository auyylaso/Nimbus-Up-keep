#include "fakelag.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"

bool Settings::FakeLag::enabled = false;
int Settings::FakeLag::value = 9;

bool Settings::FakeLag::lagSpike = false;
bool FakeLag::lagSpike = false;

bool Settings::FakeLag::States::enabled = false;
bool Settings::FakeLag::States::Standing::enabled = false;
int Settings::FakeLag::States::Standing::value = 9;
bool Settings::FakeLag::States::Moving::enabled = false;
int Settings::FakeLag::States::Moving::value = 9;
bool Settings::FakeLag::States::Air::enabled = false;
int Settings::FakeLag::States::Air::value = 9;

static int ticks = 0;
int ticksMax = 16;

void FakeLag::CreateMove(CUserCmd* cmd)
{
	if (!Settings::FakeLag::enabled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

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
	else if (Settings::FakeLag::States::enabled)
	{
		if (Settings::FakeLag::States::Air::enabled && !(localplayer->GetFlags() & FL_ONGROUND))
			CreateMove::sendPacket = ticks < 16 - Settings::FakeLag::States::Air::value;
		else if (Settings::FakeLag::States::Moving::enabled && localplayer->GetVelocity().Length() > 0.0f)
			CreateMove::sendPacket = ticks < 16 - Settings::FakeLag::States::Moving::value;
		else if (Settings::FakeLag::States::Standing::enabled)
			CreateMove::sendPacket = ticks < 16 - Settings::FakeLag::States::Standing::value;
	}
	else
		CreateMove::sendPacket = ticks < 16 - Settings::FakeLag::value;

	ticks++;
}
