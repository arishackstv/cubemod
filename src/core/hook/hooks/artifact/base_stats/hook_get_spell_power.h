#pragma once

#include <hook/hook.h>
#include <game_structures.h>

class HookGetSpellPower : public Hook
{
	static inline Hook* hook;

	static inline float* base_damage = (float*)(MemoryHelper::GetCubeBase() + 0x42E584);

	static float HOOK GetSpellPower(Player* player)
	{
		if (player->GetEntityType() != PLAYER) return hook->Trampoline(GetSpellPower)(player);

		//Patch the base damage value
		MemoryHelper::PatchMemory<float>(base_damage, 5.f + player->GetIncreasedArtifactStats(DAMAGE));

		//Calculate spell power with boosted base value
		float ret = hook->Trampoline(GetSpellPower)(player);

		//Patch base value back
		MemoryHelper::PatchMemory<float>(base_damage, 5.f);

		return ret;
	}

public:
	HookGetSpellPower() : Hook(MemoryHelper::GetCubeBase() + 0x65C70, (void*)GetSpellPower, hook)
	{}
};