#pragma once

#include <string>
#include <vector>

namespace NameChanger
{
	enum class NC_Type
	{
		NC_NORMAL,
	};

	extern int changes;
	extern NC_Type type;
	extern std::string origName;

	void SetName(const char* name);

	//Hooks
	void BeginFrame(float frameTime);
};
