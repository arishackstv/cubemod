#pragma once

#include <list>
#include <thread>
#include <intrin.h>
#include "../memory/memory_helper.h"

#define MAX_INSTRUCTION_LENGTH 15

class Hook;

//Shit gets weird in here be careful
class VectoredExceptionHandler
{
	static inline std::list<Hook*> registered_hooks;

	static inline Hook* pending_hook;

	static inline long HandleException(_EXCEPTION_POINTERS* info);

public:
	VectoredExceptionHandler()
	{
		AddVectoredExceptionHandler(true, (PVECTORED_EXCEPTION_HANDLER)HandleException);
	}

	static inline void RegisterHook(Hook* hook);
};

class Hook
{
private:
	const static inline uint8_t ABSOLUTE_JMP_BYTES[]{0xFF, 0x25, 0x00, 0x00, 0x00, 0x00};

	static inline VectoredExceptionHandler exception_handler;

	void* original_function_address;
	void* hook_function_address;

	uint8_t trampoline[MAX_INSTRUCTION_LENGTH * 2];
	int64_t trampoline_length;

	uint8_t original_byte;

	void PlaceBreakpoint()
	{
		original_byte = *(uint8_t*)original_function_address;
		MemoryHelper::PatchMemory<uint8_t>(original_function_address, 0xCC);
	}

	void RemoveBreakpoint()
	{
		MemoryHelper::PatchMemory<uint8_t>(original_function_address, original_byte);
	}

public:
	Hook(uint64_t original_function_address, void* hook_function_address, Hook*& hook_store)
	{
		Hook::original_function_address = (void*)original_function_address;
		Hook::hook_function_address = hook_function_address;
		hook_store = this;

		exception_handler.RegisterHook(this);
	}

	template<typename T>
	T Trampoline(T type)
	{
		return (T)(void*)trampoline;
	}

	friend class VectoredExceptionHandler; //Sure is my friend
};

//Registration involves setting a breakpoint, getting first instruction length, and creating a trampoline
void VectoredExceptionHandler::RegisterHook(Hook* hook)
{
	//Set the pending hook to be registered
	pending_hook = hook;

	//Place breakpoint
	hook->PlaceBreakpoint();

	//Execute the first instruction in the original function (function won't actually get to do anything because the thread is killed after first instruction)
	auto thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)hook->original_function_address, nullptr, 0, 0);
	WaitForSingleObject(thread, INFINITE);

	//Don't need that anymore
	pending_hook = nullptr;

	if (hook->trampoline_length > MAX_INSTRUCTION_LENGTH || hook->trampoline_length < 0)
	{
		//This is like PROBABLY a jump, just go there instead
		uint64_t jmp_address = (uint64_t)hook->original_function_address + hook->trampoline_length;

		memcpy(hook->trampoline, Hook::ABSOLUTE_JMP_BYTES, sizeof(Hook::ABSOLUTE_JMP_BYTES));
		memcpy(&hook->trampoline[sizeof(Hook::ABSOLUTE_JMP_BYTES)], &jmp_address, sizeof(jmp_address));
	}
	else
	{
		//Setup trampoline for hook
		memcpy(hook->trampoline, hook->original_function_address, hook->trampoline_length);

		//Jump back into original function
		auto jmp_address = (uint64_t)hook->original_function_address + hook->trampoline_length;
		memcpy(&hook->trampoline[hook->trampoline_length], Hook::ABSOLUTE_JMP_BYTES, sizeof(Hook::ABSOLUTE_JMP_BYTES));
		memcpy(&hook->trampoline[hook->trampoline_length + sizeof(Hook::ABSOLUTE_JMP_BYTES)], &jmp_address, sizeof(jmp_address));
	}

	//Make trampoline executable
	DWORD old;
	VirtualProtect(hook->trampoline, sizeof(hook->trampoline), PAGE_EXECUTE_READWRITE, &old);

	//Now we can actually use it yay
	registered_hooks.push_back(hook);
	hook->PlaceBreakpoint();
}

long VectoredExceptionHandler::HandleException(_EXCEPTION_POINTERS* info)
{
	auto except_code = info->ExceptionRecord->ExceptionCode;
	auto except_address = info->ExceptionRecord->ExceptionAddress;

	if (except_code == STATUS_BREAKPOINT)
	{
		//Handle any potential pending hooks
		if (pending_hook && pending_hook->original_function_address == except_address)
		{
			//Remove the breakpoint and break on the next instruction
			pending_hook->RemoveBreakpoint();

			info->ContextRecord->Rip = (uint64_t)pending_hook->original_function_address;
			info->ContextRecord->EFlags |= 0x100;
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		else
		{
			//Standard hook redirection
			for (Hook* hook : registered_hooks)
			{
				if (except_address != hook->original_function_address) continue;

				//Redirect execution to hook
				info->ContextRecord->Rip = (uint64_t)hook->hook_function_address;
				return EXCEPTION_CONTINUE_EXECUTION;
			}
		}
	}
	else if (except_code == STATUS_SINGLE_STEP && pending_hook)
	{
		//Catch this right after the first instruction is called and get the length
		pending_hook->trampoline_length = (uint64_t)except_address - (uint64_t)pending_hook->original_function_address;

		//Kill the thread as it was never meant to live for more than one instruction anyhow
		ExitThread(0); //die
	}

	return EXCEPTION_CONTINUE_SEARCH;
}