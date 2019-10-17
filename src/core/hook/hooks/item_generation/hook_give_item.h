#pragma once

#include "../../hook.h"
#include "../../../game_structures.h"

class HookGiveItem : public Hook
{
	static inline Hook* hook;

	//Using this to hook crafting because the actual crafting hook didn't work for some reason and I'm lazy
	static void __fastcall GivePlayerItem(__int64* a1, Item* item, int a3, __int64 a4)
	{
		auto craft_item_return_address = (void*)(MemoryHelper::GetCubeBase() + 0x9031C);

		//Should only modify newly crafted items
		if (item->IsEquipment() && _ReturnAddress() == craft_item_return_address)
		{
			//Keep it interesting
			static auto modifier_generator = std::bind(std::uniform_int_distribution<uint32_t>(0, 0xFFFFFFFF), std::default_random_engine());
			item->SetModifier(modifier_generator());

			item->ModifyRarity();
		}

		hook->Trampoline(GivePlayerItem)(a1, item, a3, a4);
	}

public:
	HookGiveItem() : Hook(MemoryHelper::GetCubeBase() + 0x4E810, (void*)GivePlayerItem, hook)
	{}
};