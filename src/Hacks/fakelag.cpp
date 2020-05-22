#include "fakelag.h"

#include "../Hooks/hooks.h"
#include "../Utils/entity.h"
#include "../interfaces.h"
#include "../settings.h"

// Variables responsible for limiting the maximum chokable ticks.
static int ticks = 0;
int ticksMax = 16;

// Runs the fakelag process every tick.
void FakeLag::CreateMove(CUserCmd *cmd)
{
	if (!Settings::FakeLag::enabled)
		return;

	if (Settings::FakeDuck::enabled && inputSystem->IsButtonDown(Settings::FakeDuck::key))
		return;

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->GetAlive())
		return;

	if (localplayer->GetVelocity().Length() < 0.1f)
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
	else if (Settings::FakeLag::onPeek)
	{
		for (int i = 1; i < globalVars->GetMaxClients(); ++i)
		{
			C_BasePlayer *player = (C_BasePlayer *)entityList->GetClientEntity(i);

			if (!player || player == localplayer || player->GetDormant() || !player->GetAlive() || player->GetImmune() || Entity::IsTeamMate(player, localplayer))
				continue;

			CreateMove::sendPacket = !Entity::IsSpotVisible(player, player->GetVecOrigin());
		}
	}
	else
		CreateMove::sendPacket = ticks < 16 - Settings::FakeLag::value;

	ticks++;
}
