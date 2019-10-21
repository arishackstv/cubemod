#pragma once

#include <hook/hook.h>
#include <game_structures.h>

class HookGetItemModifier : public Hook
{
	static inline Hook* hook;

	static float HOOK GetItemModifier(Item* item, Entity* entity, int blocks)
	{
		auto modifier = hook->Trampoline(GetItemModifier)(item, entity, blocks);

		//Players use item region thing, mobs use themselves vs region center
		auto multiplier = item->IsEquipment() ? entity->GetRegionMulitplier(entity, item) : 1.f;

		return modifier * multiplier;
	}

public:
	HookGetItemModifier() : Hook(MemoryHelper::GetCubeBase() + 0x109C50, (void*)GetItemModifier, hook)
	{}
};
