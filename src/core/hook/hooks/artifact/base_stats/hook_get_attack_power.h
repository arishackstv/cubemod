#pragma once

#include <hook/hook.h>
#include <game_structures.h>

class HookGetAttackPower : public Hook
{
	static inline Hook* hook;

	static inline float* base_damage = (float*)(MemoryHelper::GetCubeBase() + 0x42E584);

	static float HOOK GetAttackPower(Player* player)
	{
		if (player->GetEntityType() != PLAYER) return hook->Trampoline(GetAttackPower)(player);

		//Patch the base damage value
		MemoryHelper::PatchMemory<float>(base_damage, 5.f + player->GetIncreasedArtifactStats(DAMAGE));

		//Calculate attack power with boosted base value
		float ret = hook->Trampoline(GetAttackPower)(player);

		//Patch base value back
		MemoryHelper::PatchMemory<float>(base_damage, 5.f);

		return ret;
	}

public:
	HookGetAttackPower() : Hook(MemoryHelper::GetCubeBase() + 0x4FA70, (void*)GetAttackPower, hook)
	{}
};