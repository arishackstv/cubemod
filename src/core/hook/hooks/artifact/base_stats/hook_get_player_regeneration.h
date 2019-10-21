#pragma once

#include <hook/hook.h>
#include <game_structures.h>

class HookGetPlayerRegeneration : public Hook
{
	static inline Hook* hook;

	static float HOOK GetRegeneration(Entity* entity)
	{
		auto ret = hook->Trampoline(GetRegeneration)(entity);
		if (entity->GetEntityType() != PLAYER) return ret;

		return ret + ((Player*)entity)->GetIncreasedArtifactStats(REGENERATION) / 100.f;
	}

public:
	HookGetPlayerRegeneration() : Hook(MemoryHelper::GetCubeBase() + 0x647B0, (void*)GetRegeneration, hook)
	{}
};