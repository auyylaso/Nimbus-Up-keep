#include "override.h"

#include "../Utils/xorstring.h"
#include "../interfaces.h"
#include "../settings.h"

bool Settings::Override::enabled = false;

// Full-bright
bool Settings::Override::Bright::enabled = false;

// Fog
bool Settings::Override::Fog::enabled = false;

// Aspect ratio
bool Settings::Override::Aspect::enabled = false;

// Gravity
bool Settings::Override::Gravity::enabled = false;
int Settings::Override::Gravity::amount = 800;

// Viewmodel
bool Settings::Override::Viewmodel::enabled = false;
float Settings::Override::Viewmodel::fov = 68;
float Settings::Override::Viewmodel::x = 1;
float Settings::Override::Viewmodel::y = 1;
float Settings::Override::Viewmodel::z = 1;

// Aspect ratio
float Settings::Override::Aspect::var = 1;
// Not yet implemented!
void Override::FrameStageNotify(ClientFrameStage_t stage)
{
	if (!Settings::Override::enabled)
		return;

	if (!engine->IsInGame())
		return;

	static ConVar *svcheats = cvar->FindVar("sv_cheats");
	svcheats->SetValue(1);

	static ConVar *fullbright = cvar->FindVar("mat_fullbright");
	static ConVar *fogoverride = cvar->FindVar("fog_override");

	fullbright->SetValue(Settings::Override::Bright::enabled);
	fogoverride->SetValue(Settings::Override::Fog::enabled);

	if (Settings::Override::Gravity::enabled)
	{
		static ConVar *invertedragdoll = cvar->FindVar("cl_ragdoll_gravity");
		invertedragdoll->flags &= ~FCVAR_CHEAT;
		invertedragdoll->SetValue(Settings::Override::Gravity::amount);
	}
	else
	{
		static ConVar *invertedragdoll = cvar->FindVar("cl_ragdoll_gravity");
		invertedragdoll->flags &= ~FCVAR_CHEAT;
		invertedragdoll->SetValue(800);
	}

	if (Settings::Override::Viewmodel::enabled)
	{
		static ConVar *minspec = cvar->FindVar(XORSTR("sv_competitive_minspec"));
		minspec->fnChangeCallback = 0;
		minspec->SetValue(0);
	}
	else
	{
		static ConVar *minspec = cvar->FindVar(XORSTR("sv_competitive_minspec"));
		minspec->fnChangeCallback = 0;
		minspec->SetValue(1);
	}

	if (Settings::Override::Viewmodel::enabled)
	{
		static ConVar *viewmodelfov = cvar->FindVar("viewmodel_fov");
		static ConVar *viewmodelx = cvar->FindVar("viewmodel_offset_x");
		static ConVar *viewmodely = cvar->FindVar("viewmodel_offset_y");
		static ConVar *viewmodelz = cvar->FindVar("viewmodel_offset_z");
		static ConVar *aspect = cvar->FindVar("r_aspectratio");

		float ratio = (Settings::Override::Aspect::var * 0.1) / 2;

		viewmodelfov->SetValue(Settings::Override::Viewmodel::fov);
		viewmodelx->SetValue(Settings::Override::Viewmodel::x);
		viewmodely->SetValue(Settings::Override::Viewmodel::y);
		viewmodelz->SetValue(Settings::Override::Viewmodel::z);

		if (ratio > 0.001)
			aspect->SetValue(ratio);
		else
			aspect->SetValue((35 * 0.1f) / 2);
	}
}
