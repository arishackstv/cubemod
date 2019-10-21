#pragma once

#include <algorithm>

extern "C"
{
#include <xed/include/xed-interface.h>
}

#include <memory/memory_helper.h>

#define MAX_INSTRUCTION_LENGTH 15
#define JMP_LENGTH 6
#define ABSOLUTE_JMP_LENGTH 14
#define MAX_TRAMPOLINE_LENGTH MAX_INSTRUCTION_LENGTH * 3
#define HOOK __fastcall

class Hook
{
private:
	const static inline uint8_t JMP_BYTES[]{0xFF, 0x25, 0x00, 0x00, 0x00, 0x00};

	static inline std::vector<Hook*> installed_hooks;

	void* original_function_address;
	void* hook_function_address;

	std::vector<uint64_t> used_empty_space;

	uint8_t trampoline[MAX_TRAMPOLINE_LENGTH];

	uint8_t original_bytes[MAX_INSTRUCTION_LENGTH + JMP_LENGTH];
	int original_bytes_length = 0;

public:
	Hook(uint64_t original_function_address, void* hook_function_address, Hook*&hook_store)
	{
		Hook::original_function_address = (void*)original_function_address;
		Hook::hook_function_address = hook_function_address;
		hook_store = this;

		InstallHook();
	}

	~Hook()
	{
		UninstallHook();
	}

	template<typename T>
	T Trampoline(T type)
	{
		return (T)(void*)trampoline;
	}

private:
	void WriteAbsoluteJMP(void* from, void* to)
	{
		MemoryHelper::PatchMemory(from, (void*)JMP_BYTES, sizeof(JMP_BYTES));
		MemoryHelper::PatchMemory<void*>((uint64_t)from + sizeof(JMP_BYTES), to);
	}

	void WriteIndirectJMP(void* from, void* to)
	{
		auto store_location = MemoryHelper::FindPattern("CC CC CC CC CC CC CC CC");
		MemoryHelper::PatchMemory<void*>(store_location, to);
		used_empty_space.push_back(store_location);

		auto store_offset = store_location - ((uint64_t)from + sizeof(JMP_BYTES));
		MemoryHelper::PatchMemory(from, (void*)JMP_BYTES, sizeof(JMP_BYTES));
		MemoryHelper::PatchMemory<int32_t>(((uint64_t)from + 2), (int32_t)store_offset);
	}

	/**
	 * Checks for any already-installed hooks at a location, most recent first
	 * @param hooked_location
	 * @return a pointer to the most recent hook, or nullptr
	 */
	static Hook* GetLatestHook(void* hooked_location)
	{
		for (auto r = installed_hooks.rbegin(); r != installed_hooks.rend(); r++)
		{
			auto hook = *r;
			if (hook->original_function_address == hooked_location) return hook;
		}

		return nullptr;
	}

	bool InstallHook()
	{
		xed_tables_init();
		uint8_t* function_address = (uint8_t*)original_function_address;
		uint8_t* trampoline_address = trampoline;

		while (original_bytes_length < JMP_LENGTH)
		{
			xed_decoded_inst_t xedd{};
			xed_decoded_inst_set_mode(&xedd, XED_MACHINE_MODE_LONG_64, XED_ADDRESS_WIDTH_64b);

			xed_decode(&xedd, function_address, MAX_INSTRUCTION_LENGTH);
			auto inst_len = xed_decoded_inst_get_length(&xedd);

			//Suport for functions that jump immediately
			if (original_bytes_length == 0 && xed_inst_iform_enum(xedd._inst) == XED_IFORM_JMP_MEMv)
			{
				auto installed_hook = GetLatestHook(function_address);

				auto relative_location = *(int32_t*)(function_address + inst_len - sizeof(int32_t));
				auto jump_location = *(uint64_t*)(function_address + inst_len + relative_location);

				//Replace jump in trampoline
				WriteAbsoluteJMP(trampoline_address, (void*)jump_location);

				//If the hook jumps immediately just put that in the trampoline
				trampoline_address += ABSOLUTE_JMP_LENGTH;
				original_bytes_length = inst_len;
				break;
			}

			//Copy the instruction to the trampoline
			memcpy(trampoline_address, function_address, inst_len);

			//Increment addresses and length each instruction
			trampoline_address += inst_len;
			function_address += inst_len;
			original_bytes_length += inst_len;
		}

		//Store the original bytes
		memcpy(original_bytes, original_function_address, original_bytes_length);

		//Have trampoline jump back into function
		WriteAbsoluteJMP(trampoline_address, (void*)((uint64_t)original_function_address + original_bytes_length));

		//Replace function bytes with a jump to our hook
		WriteIndirectJMP(original_function_address, hook_function_address);

		//Make trampoline executable
		DWORD old;
		VirtualProtect(trampoline, sizeof(trampoline), PAGE_EXECUTE_READWRITE, &old);

		//Yay we did it
		installed_hooks.push_back(this);
		return true;
	}

	void UninstallHook()
	{
		//Restore original function
		MemoryHelper::PatchMemory(original_function_address, original_bytes, original_bytes_length);

		//Replace the empty space that we used with this hook
		for (auto location : used_empty_space) MemoryHelper::PatchMemory<uint64_t>(location, 0xCCCCCCCCCCCCCCCC);
		used_empty_space.clear();

		installed_hooks.erase(std::find(installed_hooks.begin(), installed_hooks.end(), this));
	}
};