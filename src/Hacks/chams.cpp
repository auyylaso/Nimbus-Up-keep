#include "chams.h"
#include "lagcomp.h"

#include "../Hooks/hooks.h"
#include "../Utils/entity.h"
#include "../Utils/xorstring.h"
#include "../interfaces.h"
#include "../settings.h"

IMaterial *materialChams;
IMaterial *materialChamsIgnorez;
IMaterial *materialChamsFlat;
IMaterial *materialChamsFlatIgnorez;
IMaterial *materialPasteChams;
IMaterial *materialPasteChamsIgnorez;
IMaterial *materialChamsArms;
IMaterial *materialChamsWeapons;

// Main material variables
IMaterial *visible_material = nullptr;
IMaterial *hidden_material = nullptr;

typedef void (*DrawModelExecuteFn)(void *, void *, void *, const ModelRenderInfo_t &, matrix3x4_t *);

/*
// This is for setting the new chams's properties in a cleaner way.
// Allows more modular ways to edit properties of the new chams.
// - !Doesn't work! -
static std::string GetEnvMat(float a, float b, float c)
{
	return "[" + std::to_string(a) + " " + std::to_string(b) + " " + std::to_string(c) + "]";
}
*/

static void DrawPlayer(void *thisptr, void *context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
{
	if (!Settings::ESP::Chams::enabled)
		return;

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;

	C_BasePlayer *entity = (C_BasePlayer *)entityList->GetClientEntity(pInfo.entity_index);
	if (!entity || entity->GetDormant() || !entity->GetAlive())
		return;

	if (entity == localplayer && !Settings::ESP::Filters::localplayer)
		return;

	if (!Entity::IsTeamMate(entity, localplayer) && !Settings::ESP::Filters::enemies)
		return;

	if (entity != localplayer && Entity::IsTeamMate(entity, localplayer) && !Settings::ESP::Filters::allies)
		return;

	visible_material->AlphaModulate(1.0f);
	hidden_material->AlphaModulate(1.0f);

	if (entity == localplayer)
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::localplayerColor.Color(entity));
		Color color = visColor;
		color *= 0.45f;

		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);

		visible_material->AlphaModulate(Settings::ESP::Chams::localplayerColor.Color(entity).Value.w);
		hidden_material->AlphaModulate(Settings::ESP::Chams::localplayerColor.Color(entity).Value.w);
	}
	else if (Entity::IsTeamMate(entity, localplayer))
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::allyVisibleColor.Color(entity));
		Color color = Color::FromImColor(Settings::ESP::Chams::allyColor.Color(entity));

		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);
	}
	else if (!Entity::IsTeamMate(entity, localplayer))
	{
		Color visColor = Color::FromImColor(Settings::ESP::Chams::enemyVisibleColor.Color(entity));
		Color color = Color::FromImColor(Settings::ESP::Chams::enemyColor.Color(entity));

		visible_material->ColorModulate(visColor);
		hidden_material->ColorModulate(color);
	}
	else
	{
		return;
	}

	if (entity->GetImmune())
	{
		visible_material->AlphaModulate(0.5f);
		hidden_material->AlphaModulate(0.5f);
	}

	/*
	// I figured that it's easier to read more shorter lines, than one long line. Sorry, if this upsets you. Chad "case" gang > Virgin "if" gang.
	if (!Settings::ESP::Filters::legit && (Settings::ESP::Chams::type == ChamsType::CHAMS_XQZ || Settings::ESP::Chams::type == ChamsType::CHAMS_FLAT_XQZ || Settings::ESP::Chams::type == ChamsType::CHAMS_PASTE_XQZ))
	{
		modelRender->ForcedMaterialOverride(hidden_material);
		modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, pCustomBoneToWorld);
	}
	*/

	if (!Settings::ESP::Filters::legit)
	{
		switch (Settings::ESP::Chams::type)
		{
			case ChamsType::CHAMS_XQZ:
			case ChamsType::CHAMS_FLAT_XQZ:
			case ChamsType::CHAMS_PASTE_XQZ:
				modelRender->ForcedMaterialOverride(hidden_material);
				modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, pCustomBoneToWorld);
				break;

			default:
				break;
		}
	}

	modelRender->ForcedMaterialOverride(visible_material);
	// No need to call DME again, it already gets called in DrawModelExecute.cpp
}

static void DrawRecord(void *thisptr, void *context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
{
	if (!Settings::LagComp::enabled)
		return;

	if (!Settings::ESP::Backtrack::enabled)
		return;

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer)
		return;

	IMaterial *material = materialChams;
	Color color = Color(192, 192, 192, 128);

	material->ColorModulate(color);
	material->AlphaModulate(0.2f);

	for (auto &frame : LagComp::lagCompTicks)
	{
		for (auto &ticks : frame.records)
		{
			if (pInfo.entity_index < engine->GetMaxClients() && entityList->GetClientEntity(pInfo.entity_index) == ticks.entity)
			{
				auto tick_difference = (globalVars->tickcount - frame.tickCount);
				if (tick_difference <= 1) continue;

				material->ColorModulate(color);
				material->AlphaModulate(0.2f);

				modelRender->ForcedMaterialOverride(material);
				modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, (matrix3x4_t *)ticks.bone_matrix);
				modelRender->ForcedMaterialOverride(nullptr);
			}
		}
	}
}

static void DrawWeapon(const ModelRenderInfo_t &pInfo)
{
	if (!Settings::ESP::Chams::Weapon::enabled)
		return;

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);
	IMaterial *mat = materialChamsWeapons;

	if (!Settings::ESP::Chams::Weapon::enabled)
		mat = material->FindMaterial(modelName.c_str(), TEXTURE_GROUP_MODEL);

	mat->ColorModulate(Settings::ESP::Chams::Weapon::color.Color());
	mat->AlphaModulate(Settings::ESP::Chams::Weapon::color.Color().Value.w);

	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Settings::ESP::Chams::Weapon::type == WeaponType::WIREFRAME);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings::ESP::Chams::Weapon::type == WeaponType::NONE);
	modelRender->ForcedMaterialOverride(mat);
}

static void DrawArms(const ModelRenderInfo_t &pInfo)
{
	if (!Settings::ESP::Chams::Arms::enabled)
		return;

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);
	IMaterial *mat = materialChamsArms;

	if (!Settings::ESP::Chams::Arms::enabled)
		mat = material->FindMaterial(modelName.c_str(), TEXTURE_GROUP_MODEL);

	mat->ColorModulate(Settings::ESP::Chams::Arms::color.Color());
	mat->AlphaModulate(Settings::ESP::Chams::Arms::color.Color().Value.w);

	mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Settings::ESP::Chams::Arms::type == ArmsType::WIREFRAME);
	mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings::ESP::Chams::Arms::type == ArmsType::NONE);
	modelRender->ForcedMaterialOverride(mat);
}

void Chams::DrawModelExecute(void *thisptr, void *context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
{
	if (!engine->IsInGame())
		return;

	if (!Settings::ESP::enabled)
		return;

	if (!pInfo.pModel)
		return;

	static bool materialsCreated = false;
	if (!materialsCreated)
	{
		materialChams = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, true, true, true, true);
		materialChamsIgnorez = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), true, true, true, true, true);
		materialChamsFlat = Util::CreateMaterial(XORSTR("UnlitGeneric"), XORSTR("VGUI/white_additive"), false, true, true, true, true);
		materialChamsFlatIgnorez = Util::CreateMaterial(XORSTR("UnlitGeneric"), XORSTR("VGUI/white_additive"), true, true, true, true, true);
		materialChamsArms = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, true, true, true, true);
		materialChamsWeapons = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, true, true, true, true);
		materialPasteChams = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), false, true, true, false, true, false, XORSTR("models/effects/cube_white"), true, XORSTR("[0 0.26 1]"), true, XORSTR("[0 1 2]"), 0.8, true, true, false, true);
		materialPasteChamsIgnorez = Util::CreateMaterial(XORSTR("VertexLitGeneric"), XORSTR("VGUI/white_additive"), true, true, true, false, true, false, XORSTR("models/effects/cube_white"), true, XORSTR("[0 0.26 1]"), true, XORSTR("[0 1 2]"), 0.8, true, true, false, true);
		materialsCreated = true;
	}

	// Moved this switch, because now I don't have to make checks in every function.
	switch (Settings::ESP::Chams::type)
	{
		case ChamsType::CHAMS:
		case ChamsType::CHAMS_XQZ:
			visible_material = materialChams;
			hidden_material = materialChamsIgnorez;
			break;
		case ChamsType::CHAMS_FLAT:
		case ChamsType::CHAMS_FLAT_XQZ:
			visible_material = materialChamsFlat;
			hidden_material = materialChamsFlatIgnorez;
			break;
		case ChamsType::CHAMS_PASTE:
		case ChamsType::CHAMS_PASTE_XQZ:
			visible_material = materialPasteChams;
			hidden_material = materialPasteChamsIgnorez;
			break;
	}

	std::string modelName = modelInfo->GetModelName(pInfo.pModel);

	if (modelName.find(XORSTR("models/player")) != std::string::npos)
	{
		DrawPlayer(thisptr, context, state, pInfo, pCustomBoneToWorld);
		DrawRecord(thisptr, context, state, pInfo, pCustomBoneToWorld);
	}
	else if (modelName.find(XORSTR("arms")) != std::string::npos)
		DrawArms(pInfo);
	else if (modelName.find(XORSTR("weapon")) != std::string::npos)
		DrawWeapon(pInfo);
}
