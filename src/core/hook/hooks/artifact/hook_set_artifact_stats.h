#pragma once

#include <hook/hook.h>
#include <game_structures.h>

class HookSetArtifactStats : public Hook
{
	static inline Hook* hook;

	static uint64_t HOOK SetArtifactStats(Player* player)
	{
		auto ret = hook->Trampoline(SetArtifactStats)(player);

		Main::GetInstance().GetLocalPlayer()->UpdateNetworkedArtifactStats();

		return ret;
	}

public:
	HookSetArtifactStats() : Hook(MemoryHelper::GetCubeBase() + 0x66850, (void*)SetArtifactStats, hook)
	{}
};