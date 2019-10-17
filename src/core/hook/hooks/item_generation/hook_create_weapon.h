#pragma once

#include "../../hook.h"
#include "../../../game_structures.h"

class HookCreateWeapon : public Hook
{
	static inline Hook* hook;

	static Item* __fastcall CreateWeapon(void* a1, void* a2, int8_t a3, int32_t a4)
	{
		auto item = hook->Trampoline(CreateWeapon)(a1, a2, a3, a4);

		//Check the return address to avoid lowering rates for shop items
		auto create_shop_offer_weapon_address = (void*)(MemoryHelper::GetCubeBase() + 0x2AD0A3);
		item->ModifyRarity(_ReturnAddress() == create_shop_offer_weapon_address);

		return item;
	}

public:
	HookCreateWeapon() : Hook(MemoryHelper::GetCubeBase() + 0x2D15D0, (void*)CreateWeapon, hook)
	{}
};