#pragma once

#include "../../hook.h"
#include "../../../game_structures.h"

class HookGetItemCrit : public Hook
{
	static inline Hook* hook;

	//Crit don't scale well
	static float __fastcall GetItemCrit(Item* item, Entity* entity)
	{
		auto regen = hook->Trampoline(GetItemCrit)(item, entity);

		auto multiplier = entity->GetRegionMulitplier(entity, item);
		auto adjusted_crit = regen * (1.0 / multiplier);

		return adjusted_crit * 0.9;
	}

public:
	HookGetItemCrit() : Hook(MemoryHelper::GetCubeBase() + 0x1090F0, (void*)GetItemCrit, hook)
	{}
};