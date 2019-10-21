#pragma once

#include <hook/hook.h>
#include <game_structures.h>

class HookConcatArtifactSuffix : public Hook
{
	static inline Hook* hook;

	static uint64_t HOOK ConcatArtifactSuffix(void* a1, wchar_t* str)
	{
		//If it's called from that spot in the inventory display
		if (__builtin_return_address(0) == (void*)(MemoryHelper::GetCubeBase() + 0x275663))
		{
			//Remove the extra stuff for our artifact display
			switch (artifact_index)
			{
				case 0:
				case 1:
				case 2:
				case 6:
					str = L"";
					break;
			}

			//This is always called for each artifact, so the artifact index should stay accurate
			artifact_index++;
			if (artifact_index == 7) artifact_index = 0;
		}

		return hook->Trampoline(ConcatArtifactSuffix)(a1, str);
	}

public:
	HookConcatArtifactSuffix() : Hook(MemoryHelper::GetCubeBase() + 0x486B0, (void*)ConcatArtifactSuffix, hook)
	{}

	static inline int artifact_index;
};