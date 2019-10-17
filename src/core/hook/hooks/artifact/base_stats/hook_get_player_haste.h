#pragma once

#include "../../../hook.h"
#include "../../../../game_structures.h"

class HookGetPlayerHaste : public Hook
{
	static inline Hook* hook;

	static float __fastcall GetHaste(Entity* entity)
	{
		auto ret = hook->Trampoline(GetHaste)(entity);
		if (entity->GetEntityType() != PLAYER) return ret;

		return ret + ((Player*)entity)->GetIncreasedArtifactStats(HASTE) / 100.f;
	}

public:
	HookGetPlayerHaste() : Hook(MemoryHelper::GetCubeBase() + 0x66460, (void*)GetHaste, hook)
	{}
};