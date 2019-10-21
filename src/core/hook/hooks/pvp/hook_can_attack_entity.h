#pragma once

#include <hook/hook.h>
#include <game_structures.h>

class HookIsEnemy : public Hook
{
	static inline Hook* hook;

	static bool HOOK IsEntityEnemy(Entity* entity_1, Entity* entity_2)
	{
		if (entity_1 != entity_2 && entity_1->GetEntityType() == EntityType::PLAYER && entity_2->GetEntityType() == EntityType::PLAYER)
		{
			return Main::GetInstance().CanPvP();
		}

		return hook->Trampoline(IsEntityEnemy)(entity_1, entity_2);
	}

public:
	HookIsEnemy() : Hook(MemoryHelper::GetCubeBase() + 0x50550, (void*)IsEntityEnemy, hook)
	{}
};