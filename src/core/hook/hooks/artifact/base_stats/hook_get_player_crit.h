#pragma once

#include <hook/hook.h>
#include <game_structures.h>

class HookGetPlayerCrit : public Hook
{
	static inline Hook* hook;

	static float HOOK GetPlayerCrit(Entity* entity, uint8_t* a2, char a3)
	{
		auto ret = hook->Trampoline(GetPlayerCrit)(entity, a2, a3);
		if (entity->GetEntityType() != PLAYER) return ret;

		return ret + ((Player*)entity)->GetIncreasedArtifactStats(CRIT) / 100.f;
	}

public:
	HookGetPlayerCrit() : Hook(MemoryHelper::GetCubeBase() + 0x50D30, (void*)GetPlayerCrit, hook)
	{}
};