#pragma once

#include "../../../hook.h"
#include "../../../../game_structures.h"
#include "hook_concat_artifact_suffix.h"

class Hookroundf : public Hook
{
	static inline Hook* hook;

	//This is literally only called from the artifact display thing so it's fine
	static float __fastcall cube_roundf(float f)
	{
		//Get actual artifact stats
		return Main::GetInstance().GetLocalPlayer()->GetIncreasedArtifactStats((ArtifactType)HookConcatArtifactSuffix::artifact_index, true);
	}

public:
	Hookroundf() : Hook(MemoryHelper::GetCubeBase() + 0x275760, (void*)cube_roundf, hook)
	{}
};