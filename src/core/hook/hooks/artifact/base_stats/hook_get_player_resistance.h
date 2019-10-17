#pragma once

#include "../../../hook.h"
#include "../../../../game_structures.h"

class HookGetPlayerResistance : public Hook
{
	static inline Hook* hook;

	static float __fastcall GetResistance(Player* entity)
	{
		auto ret = hook->Trampoline(GetResistance)(entity);
		if (entity->GetEntityType() != PLAYER) return ret;

		return ret + ((Player*)entity)->GetIncreasedArtifactStats(RESISTANCE);
	}

public:
	HookGetPlayerResistance() : Hook(MemoryHelper::GetCubeBase() + 0x64D90, (void*)GetResistance, hook)
	{}
};