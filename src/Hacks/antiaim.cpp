#include "antiaim.h"

#include "aimbot.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"

bool Settings::AntiAim::enabled = false;
AntiAimType Settings::AntiAim::type = AntiAimType::RAGE;

// Custom Settings
float Settings::AntiAim::yaw = 180.0f;
float Settings::AntiAim::desync = 1.0f;

bool Settings::AntiAim::States::enabled = false;
float Settings::AntiAim::States::Stand::angle = 180.0f;
float Settings::AntiAim::States::Run::angle = 180.0f;
float Settings::AntiAim::States::Air::angle = 180.0f;

AntiAimType Settings::AntiAim::States::Stand::type = AntiAimType::RAGE;
AntiAimType Settings::AntiAim::States::Run::type = AntiAimType::RAGE;
AntiAimType Settings::AntiAim::States::Air::type = AntiAimType::RAGE;

ButtonCode_t Settings::AntiAim::back = ButtonCode_t::KEY_Z;
ButtonCode_t Settings::AntiAim::left = ButtonCode_t::KEY_X;
ButtonCode_t Settings::AntiAim::right = ButtonCode_t::KEY_C;

bool Settings::AntiAim::Freestanding::enabled = false;

bool Settings::AntiAim::AutoDisable::knifeHeld = false;

bool Settings::AntiAim::LBYBreaker::enabled = false;
bool Settings::AntiAim::LBYBreaker::custom = false;
float Settings::AntiAim::LBYBreaker::offset = 180.0f;

float AntiAim::GetMaxDelta(CCSGOAnimState *animState) {

    float speedFraction = std::max(0.0f, std::min(animState->feetShuffleSpeed, 1.0f));

    float speedFactor = std::max(0.0f, std::min(1.0f, animState->feetShuffleSpeed2));

    float unk1 = ((animState->runningAccelProgress * -0.30000001) - 0.19999999) * speedFraction;
    float unk2 = unk1 + 1.0f;
    float delta;

    if (animState->duckProgress > 0)
    {
        unk2 += ((animState->duckProgress * speedFactor) * (0.5f - unk2));// - 1.f
    }

    delta = *(float*)((uintptr_t)animState + 0x3A4) * unk2;

    return delta - 0.5f;
}

// Pasted from space!hook, but I tried
static bool GetBestHeadAngle(QAngle& angle)
{
	float b, r, l;

	Vector src3D, dst3D, forward, right, up;

	trace_t tr;
	Ray_t ray, ray2, ray3;
	CTraceFilter filter;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return false;

	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	viewAngles.x = 0;

	Math::AngleVectors(viewAngles, forward, right, up);

	auto GetTargetEntity = [ & ] ( void )
	{
		float bestFov = FLT_MAX;
		C_BasePlayer* bestTarget = NULL;

		for( int i = 0; i < engine->GetMaxClients(); ++i )
		{
			C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

			if (!player
				|| player == localplayer
				|| player->GetDormant()
				|| !player->GetAlive()
				|| player->GetImmune()
				|| player->GetTeam() == localplayer->GetTeam())
				continue;

			float fov = Math::GetFov(viewAngles, Math::CalcAngle(localplayer->GetEyePosition(), player->GetEyePosition()));

			if( fov < bestFov )
			{
				bestFov = fov;
				bestTarget = player;
			}
		}

		return bestTarget;
	};

	auto target = GetTargetEntity();
	filter.pSkip = localplayer;
	src3D = localplayer->GetEyePosition();
	dst3D = src3D + (forward * 384);

	if (!target)
		return false;

	ray.Init(src3D, dst3D);
	trace->TraceRay(ray, MASK_SHOT, &filter, &tr);
	b = (tr.endpos - tr.startpos).Length();

	ray2.Init(src3D + right * 35, dst3D + right * 35);
	trace->TraceRay(ray2, MASK_SHOT, &filter, &tr);
	r = (tr.endpos - tr.startpos).Length();

	ray3.Init(src3D - right * 35, dst3D - right * 35);
	trace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	l = (tr.endpos - tr.startpos).Length();

	if (b < r && b < l && l == r) // Left = Right > Back
		return true;

	if (b > r && b > l) // Back
		angle.y -= 180;
	else if (r > l && r > b) // Right
		angle.y += 90;
	else if (r > l && r == b) // Right = Back
		angle.y += 135;
	else if (l > r && l > b) // Left
		angle.y -= 90;
	else if (l > r && l == b) // Left = Back
		angle.y -= 135;
	else
		return false;

	return true;
}

static void DoAntiAim(AntiAimType type, QAngle& angle, bool bSend, CCSGOAnimState* animState, bool directionSwitch, C_BasePlayer* localplayer, CUserCmd* cmd)
{
    #pragma region Move-State Checks
    if (Settings::AntiAim::States::enabled)
    {
        if (localplayer->GetVelocity().Length() <= 0.0f)
            type = Settings::AntiAim::States::Stand::type;
        else if (!(localplayer->GetFlags() & FL_ONGROUND))
            type = Settings::AntiAim::States::Air::type;
        else
            type = Settings::AntiAim::States::Run::type;
    }
    else
        type = Settings::AntiAim::type;
    #pragma endregion

	float maxDelta = AntiAim::GetMaxDelta(animState);
    float desync = maxDelta * Settings::AntiAim::desync;

    switch (type)
    {
    #pragma region Default Rage
    case AntiAimType::RAGE: {

        static bool yFlip = false;

        angle.x = 89.0f;
        if (yFlip)
            angle.y += directionSwitch ? maxDelta / 2 : -maxDelta / 2;
        else
            angle.y += directionSwitch ? -maxDelta / 2 + 180.0f : maxDelta / 2 + 180.0f;

        if (!bSend)
        {
            if (yFlip)
                angle.y += directionSwitch ? -maxDelta : maxDelta;
            else
                angle.y += directionSwitch ? maxDelta : -maxDelta;
        }
        else
            yFlip = !yFlip;

    } break;
    #pragma endregion
    
    #pragma region Default Legit
    case AntiAimType::LEGIT: {

        if (!bSend)
            angle.y += directionSwitch ? maxDelta : -maxDelta;

    } break;
    #pragma endregion

    #pragma region Custom AA (WIP)
    case AntiAimType::CUSTOM: {

        angle.x = 89.0f;
        
        if (Settings::AntiAim::States::enabled)
        {
            if (localplayer->GetVelocity().Length() <= 0.0f)
                angle.y += Settings::AntiAim::States::Stand::angle;
            else if (!(localplayer->GetFlags() & FL_ONGROUND))
                angle.y += Settings::AntiAim::States::Air::angle;
            else
                angle.y += Settings::AntiAim::States::Run::angle;
        }
        else
            angle.y += Settings::AntiAim::yaw;

        if (!bSend)
            angle.y += directionSwitch ? desync : -desync;
    }
    #pragma endregion
    
    default:
        break;
    }
}

void AntiAim::CreateMove(CUserCmd* cmd)
{
    #pragma region Return Checks
    if (!Settings::AntiAim::enabled)
        return;

    if (Settings::Aimbot::AimStep::enabled && Aimbot::aimStepInProgress)
        return;

    QAngle oldAngle = cmd->viewangles;
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;

    QAngle angle = cmd->viewangles;

    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;

    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon)
        return;

    if (activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_GRENADE)
    {
        C_BaseCSGrenade* csGrenade = (C_BaseCSGrenade*) activeWeapon;

        if (csGrenade->GetThrowTime() > 0.f)
            return;
    }

    if (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK || (cmd->buttons & IN_ATTACK2 && *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER))
        return;

    if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
        return;

    if (Settings::AntiAim::AutoDisable::knifeHeld && localplayer->GetAlive() && activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE)
        return;
    #pragma endregion Return Checks

    QAngle edge_angle = angle;
	bool freestanding = Settings::AntiAim::Freestanding::enabled && GetBestHeadAngle(edge_angle);

    static bool bSend = true;
    bSend = !bSend;

    #pragma region LBY Breaker
    bool needToFlick = false;

    if (Settings::AntiAim::LBYBreaker::enabled)
    {
        static float lastCheck;
        static bool lbyBreak = false;

        if (localplayer->GetVelocity().Length2D() >= 0.1f || !(localplayer->GetFlags() & FL_ONGROUND) || localplayer->GetFlags() & FL_FROZEN)
        {
            lbyBreak = false;
            lastCheck = globalVars->curtime;
        } 
        else 
        {
            if (!lbyBreak && (globalVars->curtime - lastCheck) > 0.22)
            {
                angle.y -= Settings::AntiAim::LBYBreaker::offset;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
            } 
            else if (lbyBreak && (globalVars->curtime - lastCheck) > 1.1)
            {
                angle.y -= Settings::AntiAim::LBYBreaker::offset;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
            }
        }
    }
    #pragma endregion LBY Breaker

    #pragma region Angle Switching
    static bool directionSwitch = false;

    if (inputSystem->IsButtonDown(Settings::AntiAim::left))
		directionSwitch = true;
	else if (inputSystem->IsButtonDown(Settings::AntiAim::right))
		directionSwitch = false;
    #pragma endregion Angle Switching

    #pragma region Anti-Aim
    if (needToFlick)
        CreateMove::sendPacket = false;
    else
    {
        CreateMove::sendPacket = bSend;
        static AntiAimType type;

        CCSGOAnimState* animState = localplayer->GetAnimState();

    	DoAntiAim(type, angle, bSend, animState, directionSwitch, localplayer, cmd);

        if (freestanding)
            angle.y = edge_angle.y;
    }
    #pragma endregion Anti-Aim

    Math::NormalizeAngles(angle);
    Math::ClampAngles(angle);

    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
}
