#pragma once

#include <hook/hook.h>
#include <game_structures.h>

class HookRoundArtifactThing : public Hook
{
	static inline Hook* hook;

	static float HOOK RoundArtifactThing(float f)
	{
		//If it's called from that spot in the inventory display
		if (__builtin_return_address(0) == (void*)(MemoryHelper::GetCubeBase() + 0x2755E4) || __builtin_return_address(0) == (void*)(MemoryHelper::GetCubeBase() + 0x275646))
		{
			f = Main::GetInstance().GetLocalPlayer()->GetIncreasedArtifactStats((ArtifactType)HookConcatArtifactSuffix::artifact_index, true);
		}

		return hook->Trampoline(RoundArtifactThing)(f);
	}

public:
	HookRoundArtifactThing() : Hook(MemoryHelper::GetCubeBase() + 0xA2740, (void*)RoundArtifactThing, hook)
	{}
};