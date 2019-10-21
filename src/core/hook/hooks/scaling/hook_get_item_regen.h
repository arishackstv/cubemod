#pragma once

#include <hook/hook.h>
#include <game_structures.h>

class HookGetItemRegen : public Hook
{
	static inline Hook* hook;

	//Stamina regen doesn't scale well, so just don't scale it I guess, also lowered by default cuz you can like infinite dodge and it's weird
	static float HOOK GetItemRegen(Item* item, Entity* entity)
	{
		auto regen = hook->Trampoline(GetItemRegen)(item, entity);

		auto multiplier = entity->GetRegionMulitplier(entity, item);
		auto adjusted_regen = regen * (1.f / multiplier) * 100.f;
		adjusted_regen = (1.f / log(1.06f)) * log((1.f / 9.f) * (adjusted_regen + 1.f));

		return std::max(adjusted_regen / 100.f, 0.f) * 0.95;
	}

public:
	HookGetItemRegen() : Hook(MemoryHelper::GetCubeBase() + 0x109F30, (void*)GetItemRegen, hook)
	{}
};