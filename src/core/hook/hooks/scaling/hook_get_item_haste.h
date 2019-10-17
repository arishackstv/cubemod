#pragma once

#include "../../hook.h"
#include "../../../game_structures.h"

class HookGetItemHaste : public Hook
{
	static inline Hook* hook;

	//Haste don't scale well
	static float __fastcall GetItemHaste(Item* item, Entity* entity, float a3)
	{
		auto regen = hook->Trampoline(GetItemHaste)(item, entity, a3);

		auto multiplier = entity->GetRegionMulitplier(entity, item);
		auto adjusted_haste = regen * (1.0 / multiplier);

		return adjusted_haste;
	}

public:
	HookGetItemHaste() : Hook(MemoryHelper::GetCubeBase() + 0x10A490, (void*)GetItemHaste, hook)
	{}
};