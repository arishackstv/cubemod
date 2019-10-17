#pragma once

#include "../../../hook.h"
#include "../../../../game_structures.h"

class HookGetPlayerArmor : public Hook
{
	static inline Hook* hook;

	static float __fastcall GetArmor(Entity* entity)
	{
		auto ret = hook->Trampoline(GetArmor)(entity);
		if (entity->GetEntityType() != PLAYER) return ret;

		//Just nerf base armor a bit here
		ret *= 0.9;

		return ret + ((Player*)entity)->GetIncreasedArtifactStats(ARMOR);
	}

public:
	HookGetPlayerArmor() : Hook(MemoryHelper::GetCubeBase() + 0x4F080, (void*)GetArmor, hook)
	{}
};