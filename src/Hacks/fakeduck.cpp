#include "fakeduck.h"

#include "../Hooks/hooks.h"
#include "../interfaces.h"
#include "../settings.h"

void FakeDuck::CreateMove(CUserCmd *cmd)
{
	if (!Settings::FakeDuck::enabled)
		return;

	if (!(cmd->buttons & IN_DUCK))
		return;

	CreateMove::sendPacket = false;

	static bool counter = false;
	static int counters = 0;

	if (counters == 9)
	{
		counters = 0;
		counter = !counter;
	}

	counters++;

	if (counter)
	{
		cmd->buttons |= IN_DUCK;
		CreateMove::sendPacket = true;
	}
	else
		cmd->buttons &= ~IN_DUCK;
}