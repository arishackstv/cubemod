#include <Windows.h>
#include <main.h>

void Start()
{
	Main::GetInstance().Start();
}

bool __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		Main::GetInstance().module = hinstDLL;

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, nullptr, 0, 0);

		return true;
	}

	return false;
}