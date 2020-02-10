#include "resolver.h"

#include "../ImGUI/imgui.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/SDK.h"

#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../Utils/xorstring.h"

#include "../interfaces.h"
#include "../settings.h"

#include "antiaim.h"
#include "lagcomp.h"
#include "logshots.h"

bool Settings::Resolver::resolveAll = false;
ResolverType Settings::Resolver::type = ResolverType::Nasa;
std::vector < int64_t > Resolver::Players = {};
bool bLowerBodyUpdated = false;

//bool Settings::ESP::Info::missedShots = 0;
int missedShots = (int) Settings::ESP::Info::missedShots;
float Settings::Resolver::x = 0.3f;
float Settings::Resolver::y = 0.3f;
int Settings::Resolver::RandMax = 2;
static std::array < int, 64 > oldMissedShots = {0};

//std::vector<std::pair<C_BasePlayer*, QAngle>> player_data;

void Resolver::FrameStageNotify(ClientFrameStage_t stage) 
{
    if (!engine->IsInGame())
        return;

    C_BasePlayer * localplayer = (C_BasePlayer * ) entityList->GetClientEntity(engine->GetLocalPlayer());

    if (!localplayer)
        return;

    std::vector < std::pair < C_BasePlayer * , QAngle >> player_data;

    if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START) 
	{
        for (int i = 1; i < engine->GetMaxClients(); ++i) 
		{
            C_BasePlayer * player = (C_BasePlayer * ) entityList->GetClientEntity(i);

			if (!player
				|| player == localplayer
				|| player->GetDormant()
				|| !player->GetAlive()
				|| player->GetImmune()
				|| Entity::IsTeamMate(player, localplayer))
				continue;

            IEngineClient::player_info_t entityInformation;
            engine->GetPlayerInfo(i, & entityInformation);

            CCSGOAnimState * animState = player->GetAnimState();

            if (!animState)
                continue;

            // float maxDelta = AntiAim::GetMaxDelta(animState);

            if (!Settings::Resolver::resolveAll && std::find(Resolver::Players.begin(), Resolver::Players.end(), entityInformation.xuid) == Resolver::Players.end())
                continue;

            player_data.push_back(std::pair < C_BasePlayer * , QAngle > (player, * player->GetEyeAngles()));

			switch (Settings::Resolver::type)
			{
			case ResolverType::Fuzion: {
				player->GetEyeAngles()->y = (rand() % 2) ?
					player->GetEyeAngles()->y + (AntiAim::GetMaxDelta(player->GetAnimState()) * 0.66f) :
					player->GetEyeAngles()->y - (AntiAim::GetMaxDelta(player->GetAnimState()) * 0.66f);
			} break;

			
			case ResolverType::Nasa: {

				static int missedShot = LogShots::missedShots[player->GetIndex() - 1];
				static int oldMissedShot = oldMissedShots[player->GetIndex() - 1];

				if (missedShot <= oldMissedShot)
					continue;

                static float old;

                if (old != * player->GetLowerBodyYawTarget())
                    old = * player->GetLowerBodyYawTarget();

                static float next_update[65];
                static int idx = 65;
                if (old != * player->GetLowerBodyYawTarget() || * next_update <= player->GetSimulationTime()) 
				{
                    if (player->GetVelocity().Length2D() > 0.1f && player->GetFlags() & FL_ONGROUND)
                        return;

                    next_update[idx] = player->GetSimulationTime() + globalVars->interval_per_tick + 1.1f;
                }

                switch (missedShots % 4) 
				{
					case 0:
						player->GetEyeAngles()->y = * player->GetLowerBodyYawTarget() + 45;
						break;
					case 1:
						player->GetEyeAngles()->y = * player->GetLowerBodyYawTarget() - 45;
						break;
					case 2:
						player->GetEyeAngles()->y = * player->GetLowerBodyYawTarget() - 30;
						break;
					case 3:
						player->GetEyeAngles()->y = * player->GetLowerBodyYawTarget() + 30;
						break;
                }

			} break;

			case ResolverType::skerei: 
			{
				static float ShotTime[65];
				static float storedSimtime[65];
				static Vector oldOrigin[65];

				static float * PosParams = reinterpret_cast < float * > (reinterpret_cast < uintptr_t > (player) + 0x2774);
				static bool shot = false;

				static auto * AnimState = player->GetAnimState();
				if (!AnimState || !player->GetAnimOverlay() || !PosParams)
					return;

				if ( * reinterpret_cast < float * > (reinterpret_cast < uintptr_t > (AnimState) + 0x164) < 0) //this basically the key to nospread
					*
					reinterpret_cast < float * > (reinterpret_cast < uintptr_t > (AnimState) + 0x110) = 0.f;

				static bool update = false;

				if (storedSimtime[player->GetIndex()] != player->GetSimulationTime()) 
				{
					player->GetAnimState()->lastAnimUpdateFrame = true;
					player->GetAnimState()->lastAnimUpdateTime = true;
					update = true;
				}

				if (player->GetActiveWeapon()) 
				{
					if (ShotTime[player->GetIndex()] != player->GetShotsFired()) 
					{
						shot = true;
						ShotTime[player->GetIndex()] = player->GetShotsFired();
					}
					else
						shot = false;
				}
				else 
				{
					shot = false;
					ShotTime[player->GetIndex()] = 0.f;
				}

				static auto animation_state = player->GetAnimState();
				static CCSGOAnimState * animState = player->GetAnimState();

				static float duckammount = * (float * )(animState + 0xA4);
				static float speedfraction = player->GetAnimState()->horizontalVelocity;
				static float speedfactor = player->GetAnimState()->speed;

				static float unk1 = (( * reinterpret_cast < float * > (animState + 0x11C) * -0.30000001) - 0.19999999) * speedfraction;
				static float unk2 = unk1 + 1.f;
				if (duckammount > 0) 
				{
					unk2 += ((duckammount * speedfactor) * (0.5f - unk2));
				}

				if (duckammount > 0) 
				{
					unk2 += ((duckammount * speedfactor) * (0.5f - unk2));
				}

				static auto feet_yaw = animation_state->currentFeetYaw;

				static float body_yaw = 58.f;
				animation_state->yaw;

				static auto move_yaw = 58.f;
				static auto goal_feet_yaw = animation_state->goalFeetYaw;

				static float feet_yaw_rate = animation_state->feetYawRate;
				static float feet_cucle = animation_state->feetCycle;

				static float new_body_yaw = animation_state->yaw;
				static auto body_max_rotation = AntiAim::GetMaxDelta(player->GetAnimState());

				static auto normalized_eye_abs_yaw_diff = fmod((animation_state->yaw - feet_yaw), 360.0);
				static auto body_min_rotation = -AntiAim::GetMaxDelta(player->GetAnimState());

				if (player->GetVelocity().Length2D() < 35.f) 
				{
					if (move_yaw) 
					{
						player->GetEyeAngles()->y = player->GetEyeAngles()->y + move_yaw + feet_yaw * 32.f && feet_yaw + feet_yaw_rate / 32.f;
					}
					else 
					{
						if (feet_yaw && move_yaw) 
						{
							player->GetEyeAngles()->y = player->GetEyeAngles()->y + feet_yaw + feet_yaw_rate * -32.f && goal_feet_yaw + feet_yaw / 32.f;
						}
					}
				}
				else 
				{
					if (player->GetVelocity().Length2D() > 74.f && player->GetFlags() & FL_ONGROUND) 
					{
						player->GetEyeAngles()->y = * player->GetLowerBodyYawTarget();
						Math::NormalizeYaw(player->GetEyeAngles()->y);

						if (new_body_yaw == 58.f) 
							player->GetEyeAngles()->y = player->GetEyeAngles()->y - body_yaw * -58.f + goal_feet_yaw + feet_yaw_rate + feet_yaw / 32.f;
						else if (new_body_yaw >= -58.f && new_body_yaw == body_yaw) 
							player->GetEyeAngles()->y = player->GetEyeAngles()->y - new_body_yaw / 45.f || 58.f && goal_feet_yaw - feet_yaw * 32.f;
						else if (new_body_yaw <= 58.f) 
							player->GetEyeAngles()->y = player->GetEyeAngles()->y - body_yaw * 58.f + feet_yaw / -32.f && goal_feet_yaw * 32.f;
						else if (new_body_yaw == 58.f && new_body_yaw <= 58.f) 
							player->GetEyeAngles()->y = player->GetEyeAngles()->y - goal_feet_yaw / 29.f + feet_yaw * -32.f && new_body_yaw * 58.f - body_yaw / -58.f;
						else if (new_body_yaw >= -58.f && body_yaw == 58.f) 
							player->GetEyeAngles()->y = player->GetEyeAngles()->y - new_body_yaw * 58.f - feet_yaw * -32.f && goal_feet_yaw - 32.f && feet_yaw / -32.f;
					}
					if (player->GetVelocity().Length2D() < 74.f) 
					{
						if (normalized_eye_abs_yaw_diff > 0 || normalized_eye_abs_yaw_diff == 0) 
							body_min_rotation / move_yaw / -58.f;
						else 
							body_max_rotation / move_yaw / 58.f;

						if (goal_feet_yaw <= -29.f && feet_yaw >= -29.f) 
							player->GetEyeAngles()->y = player->GetEyeAngles()->y - move_yaw / 58.f + feet_yaw - goal_feet_yaw * 29.f;
						else if (feet_yaw >= 29.f && feet_yaw_rate <= 29.f) 
							player->GetEyeAngles()->y = player->GetEyeAngles()->y + move_yaw + 58.f - feet_yaw + feet_yaw_rate / 29.f;
						else if (goal_feet_yaw >= -29.f) 
							player->GetEyeAngles()->y = player->GetEyeAngles()->y - move_yaw / 58.f + feet_yaw_rate - feet_cucle + 29.f && goal_feet_yaw * 29.f;

						if (!shot && player != localplayer && player->GetTeam() != localplayer->GetTeam()) // 1337 pitch resolver
						{
							switch ((localplayer->GetFlags() & IN_ATTACK >= 1) % 3) // i skip case 0 on purpose noob
							{
							case 1:
								PosParams[12] = 89.f;
								break;
							case 2:
								PosParams[12] = -89.f;
								break;
							}
						}

						if (shot && player != localplayer && player->GetTeam() != localplayer->GetTeam() && PosParams[12] <= -80) // ez shot pitch fix LMAO
							PosParams[12] = 89.f;

						if (update) 
						{
							storedSimtime[player->GetIndex()] = player->GetSimulationTime();
							oldOrigin[player->GetIndex()] = player->GetAbsOrigin();
						}

						player->GetEyeAngles()->y = * player->GetLowerBodyYawTarget();
						Math::NormalizeYaw(player->GetEyeAngles()->y);
					}
				}
			} break;

			case ResolverType::Zede: {
				
				int missedShot = LogShots::missedShots[player->GetIndex() - 1];
				int oldMissedShot = oldMissedShots[player->GetIndex() - 1];

				if (missedShot <= oldMissedShot)
					continue;

				player_data.push_back(std::pair<C_BasePlayer*, QAngle>(player, *player->GetEyeAngles()));

				switch (missedShot % 4)
				{
					case 0:
						player->GetEyeAngles()->y + (AntiAim::GetMaxDelta(player->GetAnimState()) * 1.0f);
						break;
					case 1:
						player->GetEyeAngles()->y - (AntiAim::GetMaxDelta(player->GetAnimState()) * 1.0f);
						break;
					case 2:
						player->GetEyeAngles()->y + (AntiAim::GetMaxDelta(player->GetAnimState()) * 0.5f);
						break;
					case 3:
						player->GetEyeAngles()->y - (AntiAim::GetMaxDelta(player->GetAnimState()) * 0.5f);
						break;

					default:
						break;
				}
			} break;

			default:
				break;
			}
        }
    } 
	else if (stage == ClientFrameStage_t::FRAME_RENDER_END) 
	{
        for (unsigned long i = 0; i < player_data.size(); i++) 
		{
            std::pair < C_BasePlayer * , QAngle > player_aa_data = player_data[i];
            * player_aa_data.first->GetEyeAngles() = player_aa_data.second;
        }

        oldMissedShots = LogShots::missedShots;
        player_data.clear();
    }
}

void Resolver::PostFrameStageNotify(ClientFrameStage_t stage) 
{}

void Resolver::FireGameEvent(IGameEvent * event) 
{
    if (!event)
        return;

    if (strcmp(event->GetName(), "player_connect_full") != 0 && strcmp(event->GetName(), "cs_game_disconnected") != 0)
        return;

    if (event->GetInt("userid") && engine->GetPlayerForUserID(event->GetInt("userid")) != engine->GetLocalPlayer())
        return;

    Resolver::Players.clear();
}