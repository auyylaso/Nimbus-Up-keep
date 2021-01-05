#include "fakeduck.h"

#include "../Hooks/hooks.h"
#include "../interfaces.h"
#include "../settings.h"

void FakeDuck::CreateMove(CUserCmd *cmd)
{
	if (!Settings::FakeDuck::enabled)
		return;

	switch (Settings::FakeDuck::type)
	{
		case FakeDuckType::HOLD:
			if (!inputSystem->IsButtonDown(Settings::FakeDuck::key))
				return;
			break;

		case FakeDuckType::TOGGLE:
			static bool keyRelease = false;
			static bool toggle = false;

			if (inputSystem->IsButtonDown(Settings::FakeDuck::key) && !keyRelease)
				toggle = !toggle;

			keyRelease = inputSystem->IsButtonDown(Settings::FakeDuck::key);

			if (!toggle)
				return;
			break;

		default:
			break;
	}

	CreateMove::sendPacket = false;

	static bool pSwitch = false;
	static int counter = 0;

	if (counter % 14 == 0)
		pSwitch = true;
	else if (counter % 14 == 6)
		CreateMove::sendPacket = true;
	else if (counter % 14 == 7)
		pSwitch = false;

	counter++;

	if (pSwitch)
		cmd->buttons |= IN_DUCK;
	else
		cmd->buttons &= ~IN_DUCK;
}

void FakeDuck::OverrideView(CViewSetup *pSetup)
{
	if (!Settings::FakeDuck::enabled)
		return;

	switch (Settings::FakeDuck::type)
	{
		case FakeDuckType::HOLD:
			if (!inputSystem->IsButtonDown(Settings::FakeDuck::key))
				return;
			break;

		case FakeDuckType::TOGGLE:
			static bool keyRelease = false;
			static bool toggle = false;

			if (inputSystem->IsButtonDown(Settings::FakeDuck::key) && !keyRelease)
				toggle = !toggle;

			keyRelease = inputSystem->IsButtonDown(Settings::FakeDuck::key);

			if (!toggle)
				return;
			break;

		default:
			break;
	}

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->GetAlive())
		return;

	pSetup->origin.z = localplayer->GetAbsOrigin().z + 64.0f;
}