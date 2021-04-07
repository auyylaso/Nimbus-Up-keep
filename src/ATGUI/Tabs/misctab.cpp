#include "misctab.h"

#include <sstream>
#include <sys/stat.h>

#include "../../config.h"
#include "../../interfaces.h"

#include "../../ImGUI/imgui_internal.h"
#include "../../Utils/xorstring.h"
#include "../../settings.h"
#include "../atgui.h"
#include "../imgui.h"

#include "../../Hacks/clantagchanger.h"
#include "../../Hacks/grenadehelper.h"
#include "../../Hacks/namechanger.h"
#include "../../Hacks/namestealer.h"
#include "../../Hacks/valvedscheck.h"

#pragma GCC diagnostic ignored "-Wformat-security"

static char nickname[127] = "";

void Misc::RenderTab()
{
	const char *voteCastTypes[] = {"Console", "TeamSideChat",  "Chat"};
	const char *strafeTypes[] = {"Forwards", "Backwards", "Left", "Right", "Rage"};
	const char *animationTypes[] = {"Static", "Marquee", "Words", "Letters"};
	const char *teams[] = {"Allies", "Enemies", "Both"};
	const char *grenadeTypes[] = {"FLASH", "SMOKE", "MOLOTOV", "HEGRENADE"};
	const char *throwTypes[] = {"NORMAL", "RUN", "JUMP", "WALK"};

	ImGui::Columns(2, nullptr, true);
	{
		ImGui::BeginChild(XORSTR("Child1"), ImVec2(0, 0), true);
		{
			ImGui::Text(XORSTR("Movement"));
			ImGui::Separator();

			ImGui::Checkbox(XORSTR("Bunny Hop"), &Settings::BHop::enabled);

			ImGui::Columns(1);
			ImGui::Separator();

			if (Settings::BHop::enabled)
			{
				ImGui::Text(XORSTR("Humanizing"));
				ImGui::Separator();
				ImGui::Columns(2, nullptr, true);
				{
					ImGui::Checkbox(XORSTR("Hop Chance"), &Settings::BHop::Chance::enabled);
					ImGui::Checkbox(XORSTR("Min Hops"), &Settings::BHop::Hops::enabledMin);
					ImGui::Checkbox(XORSTR("Max Hops"), &Settings::BHop::Hops::enabledMax);
				}
				ImGui::NextColumn();
				{
					ImGui::PushItemWidth(-1);
					ImGui::SliderInt(XORSTR("##BHOPCHANCE"), &Settings::BHop::Chance::value, 0, 100);
					ImGui::SliderInt(XORSTR("##BHOPMIN"), &Settings::BHop::Hops::Min, 0, 20);
					ImGui::SliderInt(XORSTR("##BHOPMAX"), &Settings::BHop::Hops::Max, 0, 20);
				}
				ImGui::Separator();
			}

			ImGui::Columns(1);//If you're like wtf it fixes a strange aglinement bug at least I think it's a bug - auyy

			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Auto Strafe"), &Settings::AutoStrafe::enabled);
				ImGui::Checkbox(XORSTR("Edge Jump"), &Settings::EdgeJump::enabled);
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(Settings::AutoStrafe::type == AutostrafeType::AS_RAGE ? ImGui::CalcItemWidth() : -1);
				ImGui::Combo(XORSTR("##STRAFETYPE"), (int *)&Settings::AutoStrafe::type, strafeTypes, IM_ARRAYSIZE(strafeTypes));

				if (Settings::AutoStrafe::type == AutostrafeType::AS_RAGE)
				{
					ImGui::Checkbox(XORSTR("Silent"), &Settings::AutoStrafe::silent);
				}

				ImGui::PopItemWidth();
				UI::KeyBindButton(&Settings::EdgeJump::key);
			}

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("FOV"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("FOV"), &Settings::FOVChanger::enabled);
				ImGui::Checkbox(XORSTR("Viewmodel FOV"), &Settings::FOVChanger::viewmodelEnabled);
				ImGui::Checkbox(XORSTR("Ignore Scope"), &Settings::FOVChanger::ignoreScope);
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##FOVAMOUNT"), &Settings::FOVChanger::value, 0, 180);
				ImGui::SliderFloat(XORSTR("##MODELFOVAMOUNT"), &Settings::FOVChanger::viewmodelValue, 0, 360);
				ImGui::PopItemWidth();
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("Third Person"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Enabled"), &Settings::ThirdPerson::enabled);
				ImGui::Text(XORSTR("Thirdperson Key"));
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::SliderFloat(XORSTR("##TPCAMOFFSET"), &Settings::ThirdPerson::distance, 0.f, 500.f, XORSTR("Camera Offset: %0.f"));
				ImGui::PopItemWidth();
				UI::KeyBindButton(&Settings::ThirdPerson::key);
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("Grenade Helper"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Enabled ###ghenabled"), &Settings::GrenadeHelper::enabled);
			}
			ImGui::NextColumn();
			{
				ImGui::Checkbox(XORSTR("Only matching ###match"), &Settings::GrenadeHelper::onlyMatchingInfos);
			}

			ImGui::Columns(1, nullptr, true); //If you're like wtf it fixes a strange aglinement bug at least I think it's a bug - auyy
			ImGui::Columns(2);
			{
				if (ImGui::Button(XORSTR("Aimassist"), ImVec2(-1, 0)))
					ImGui::OpenPopup(XORSTR("optionAimAssist"));
				ImGui::SetNextWindowSize(ImVec2(200, 120), ImGuiSetCond_Always);
				if (ImGui::BeginPopup(XORSTR("optionAimAssist")))
				{
					ImGui::PushItemWidth(-1);
					ImGui::Checkbox(XORSTR("Enabled"), &Settings::GrenadeHelper::aimAssist);
					ImGui::SliderFloat(XORSTR("###aimstep"), &Settings::GrenadeHelper::aimStep, 0, 10, "Speed: %0.3f");
					ImGui::SliderFloat(XORSTR("###aimfov"), &Settings::GrenadeHelper::aimFov, 0, 180, "Fov: %0.2f");
					ImGui::SliderFloat(XORSTR("###aimdistance"), &Settings::GrenadeHelper::aimDistance, 0, 100, "Distance: %0.2f");
					ImGui::PopItemWidth();
					ImGui::EndPopup();
				}
			}
			ImGui::NextColumn();
			{
				if (ImGui::Button(XORSTR("Add Info"), ImVec2(-1, 0)))
					ImGui::OpenPopup(XORSTR("addinfo_throw"));

				ImGui::SetNextWindowSize(ImVec2(565, 268), ImGuiSetCond_Always);
				if (ImGui::BeginPopup(XORSTR("addinfo_throw")))
				{
					static int throwMessageCurrent = -1;
					static char inputName[40];
					static int tType = (int)ThrowType::NORMAL;
					static int gType = (int)GrenadeType::SMOKE;

					ImGui::Columns(1);
					ImGui::PushItemWidth(500);
					ImGui::InputText("", inputName, sizeof(inputName));
					ImGui::PopItemWidth();
					ImGui::SameLine();
					if (ImGui::Button(XORSTR("Add")) && engine->IsInGame() && Settings::GrenadeHelper::actMapName.length() > 0)
					{
						C_BasePlayer *localPlayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
						if (strlen(inputName) > 0)
						{
							GrenadeInfo n = GrenadeInfo((GrenadeType)gType, localPlayer->GetEyePosition(), *localPlayer->GetVAngles(), (ThrowType)tType, inputName);
							Settings::GrenadeHelper::grenadeInfos.push_back(n);
							std::ostringstream path;
							path << GetGhConfigDirectory() << Settings::GrenadeHelper::actMapName;
							if (!DoesFileExist(path.str().c_str()))
								mkdir(path.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
							path << XORSTR("/config.json");
							Settings::SaveGrenadeInfo(path.str());
						}
						strcpy(inputName, "");
					}
					ImGui::Columns(2);
					ImGui::Combo(XORSTR("###Throwtype"), &tType, throwTypes, IM_ARRAYSIZE(throwTypes));
					ImGui::NextColumn();
					ImGui::Combo(XORSTR("###Grenadetype"), &gType, grenadeTypes, IM_ARRAYSIZE(grenadeTypes));
					ImGui::Columns(1);
					ImGui::Separator();
					ImGui::PushItemWidth(550);
					auto lambda = [](void *data, int idx, const char **out_text) {
						*out_text = Settings::GrenadeHelper::grenadeInfos.at(idx).name.c_str();
						return *out_text != nullptr;
					};
					ImGui::ListBox("", &throwMessageCurrent, lambda, nullptr, Settings::GrenadeHelper::grenadeInfos.size(), 7);
					ImGui::PopItemWidth();
					ImGui::Columns(1);
					if (ImGui::Button(XORSTR("Remove"), ImVec2(ImGui::GetWindowWidth(), 30)))
						if (throwMessageCurrent > -1 && (int)Settings::GrenadeHelper::grenadeInfos.size() > throwMessageCurrent)
						{
							Settings::GrenadeHelper::grenadeInfos.erase(Settings::GrenadeHelper::grenadeInfos.begin() + throwMessageCurrent);
							std::ostringstream path;
							path << GetGhConfigDirectory() << Settings::GrenadeHelper::actMapName;
							if (!DoesFileExist(path.str().c_str()))
								mkdir(path.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
							path << XORSTR("/config.json");
							Settings::SaveGrenadeInfo(path.str());
						}
					ImGui::EndPopup();
				}
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XORSTR("Child2"), ImVec2(0, 0), true);
		{
			ImGui::Text(XORSTR("Clantag"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::PushItemWidth(-1);
				ImGui::Checkbox(XORSTR("Enabled"), &Settings::ClanTagChanger::enabled);
				ImGui::PopItemWidth();
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				if (ImGui::Button(XORSTR("Update Clantag"), ImVec2(-1, 0)))
					ClanTagChanger::UpdateClanTagCallback();
				ImGui::PopItemWidth();
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::PushItemWidth(-1);
				ImGui::InputText(XORSTR("##CLANTAG"), Settings::ClanTagChanger::value, 30);
				ImGui::PopItemWidth();

				ImGui::ItemSize(ImVec2(0.0f, 0.0f), 0.0f);
				ImGui::Text(XORSTR("Animation Delay(ms)"));
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				if (ImGui::Combo(XORSTR("##ANIMATIONTYPE"), (int *)&Settings::ClanTagChanger::type, animationTypes, IM_ARRAYSIZE(animationTypes)))
					ClanTagChanger::UpdateClanTagCallback();
				if (ImGui::SliderInt(XORSTR("##ANIMATIONSPEED"), &Settings::ClanTagChanger::animationSpeed, 500, 2000))
					ClanTagChanger::UpdateClanTagCallback();
				ImGui::PopItemWidth();
			}
			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("Nickname"));
			ImGui::Separator();

			ImGui::InputText(XORSTR("##NICKNAME"), nickname, 127);

			ImGui::SameLine();
			if (ImGui::Button(XORSTR("Set Nickname"), ImVec2(-1, 0)))
				NameChanger::SetName(std::string(nickname).c_str());

			ImGui::Columns(3, nullptr, true);
			if (ImGui::Button(XORSTR("No Name"), ImVec2(-1, 0)))
			{
				NameChanger::changes = 0;
				NameChanger::type = NameChanger::NC_Type::NC_NORMAL;
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				if (ImGui::Checkbox(XORSTR("Name Stealer"), &Settings::NameStealer::enabled))
					NameStealer::entityId = -1;
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::Combo("", &Settings::NameStealer::team, teams, IM_ARRAYSIZE(teams));
			}

			ImGui::Columns(1);
			ImGui::Separator();
			ImGui::Text(XORSTR("Other"));
			ImGui::Separator();
			ImGui::Columns(2, nullptr, true);
			{
				ImGui::Checkbox(XORSTR("Show Votes"), &Settings::voteCast::enabled);
				ImGui::Checkbox(XORSTR("Autoblock"), &Settings::Autoblock::enabled);
				ImGui::Checkbox(XORSTR("Backtrack"), &Settings::LagComp::enabled);
				ImGui::Checkbox(XORSTR("Screenshot Cleaner"), &Settings::ScreenshotCleaner::enabled);
				ImGui::Checkbox(XORSTR("Auto Accept"), &Settings::AutoAccept::enabled);
				ImGui::Checkbox(XORSTR("Auto Defuse"), &Settings::AutoDefuse::enabled);
				ImGui::Checkbox(XORSTR("Sniper Crosshair"), &Settings::SniperCrosshair::enabled);
				ImGui::Checkbox(XORSTR("Disable post-processing"), &Settings::DisablePostProcessing::enabled);
				ImGui::Checkbox(XORSTR("Show Ranks"), &Settings::ShowRanks::enabled);
				ImGui::Checkbox(XORSTR("Silent Defuse"), &Settings::AutoDefuse::silent);
				ImGui::Checkbox(XORSTR("Auto Crouch"), &Settings::Aimbot::AutoCrouch::enabled);
			}
			ImGui::NextColumn();
			{
				ImGui::PushItemWidth(-1);
				ImGui::Combo(XORSTR("##SHOWVOTETYPE"), (int *)&Settings::voteCast::type, voteCastTypes, IM_ARRAYSIZE(voteCastTypes));
				UI::KeyBindButton(&Settings::Autoblock::key);
				ImGui::SliderFloat(XORSTR("##TRACKTIME"), &Settings::LagComp::time, 0.0f, 0.2f);
			}
			ImGui::Columns(1);
			ImGui::Separator();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(210, 85));
			if (ImGui::BeginPopupModal(XORSTR("Error###UNTRUSTED_FEATURE")))
			{
				ImGui::Text(XORSTR("You cannot use this feature on a VALVE server."));

				ImGui::Checkbox(XORSTR("This is not a VALVE server"), &ValveDSCheck::forceUT);

				if (ImGui::Button(XORSTR("OK")))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}
			ImGui::PopStyleVar();

			ImGui::EndChild();
		}
	}
}
