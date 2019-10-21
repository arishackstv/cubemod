#pragma once

#include <hook/hook.h>
#include <game_structures.h>

class HookGetMaxHitPoints : public Hook
{
	static inline Hook* hook;

	static float HOOK GetMaxHitPoints(Entity* entity)
	{
		auto ret = hook->Trampoline(GetMaxHitPoints)(entity);
		if (entity->GetEntityType() != PLAYER) return ret;

		return ret + ((Player*)entity)->GetIncreasedArtifactStats(HEALTH);
	}

public:
	HookGetMaxHitPoints() : Hook(MemoryHelper::GetCubeBase() + 0x5FAC0, (void*)GetMaxHitPoints, hook)
	{}
};