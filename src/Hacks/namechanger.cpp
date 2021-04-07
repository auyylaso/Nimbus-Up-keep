#include "namechanger.h"

#include "../Utils/util.h"
#include "../Utils/xorstring.h"
#include "../interfaces.h"

std::string NameChanger::origName = "";
int NameChanger::changes = -1;
NameChanger::NC_Type NameChanger::type = NC_Type::NC_NORMAL;

void NameChanger::SetName(const char* name)
{
	ConVar* cvar_name = cvar->FindVar(XORSTR("name"));
    cvar_name->fnChangeCallback = 0;
	cvar_name->SetValue(name);
}


void NameChanger::BeginFrame(float frameTime)
{
	if (!engine->IsInGame())
		return;

	if (changes == -1)
		return;

	long currentTime_ms = Util::GetEpochTime();
	static long timeStamp = currentTime_ms;

	if (currentTime_ms - timeStamp < 150)
		return;

	timeStamp = currentTime_ms;
	changes++;

	if (changes >= 5)
	{
		switch (NameChanger::type)
		{
			case NC_Type::NC_NORMAL:
				SetName(Util::PadStringRight(XORSTR("\230auyylaso"), strlen(XORSTR("\230auyylaso")) + Util::RandomInt(10, 50)));
				break;
		}

		changes = -1;

		return;
	}

	SetName(Util::PadStringRight(XORSTR("Broken"), strlen(XORSTR("Broken")) + changes));
}