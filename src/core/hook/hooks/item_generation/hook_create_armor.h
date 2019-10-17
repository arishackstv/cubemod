#pragma once

#include "../../hook.h"
#include "../../../game_structures.h"

class HookCreateArmor : public Hook
{
	static inline Hook* hook;

	static Item* __fastcall CreateArmor(void* a1, void* a2, int8_t a3, int32_t a4, int8_t a5)
	{
		auto item = hook->Trampoline(CreateArmor)(a1, a2, a3, a4, a5);

		//Check the return address to avoid lowering rates for shop items
		auto create_shop_offer_armor_address = (void*)(MemoryHelper::GetCubeBase() + 0x2AC7F3);
		item->ModifyRarity(_ReturnAddress() == create_shop_offer_armor_address);

		return item;
	}

public:
	HookCreateArmor() : Hook(MemoryHelper::GetCubeBase() + 0x2D0680, (void*)CreateArmor, hook)
	{}
};