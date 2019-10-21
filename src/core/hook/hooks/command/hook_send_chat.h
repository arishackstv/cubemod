#pragma once

#include <hook/hook.h>

class ChatString
{
#define MAX_STORE_LENGTH 8

	union Data
	{
		wchar_t str[MAX_STORE_LENGTH];
		wchar_t* str_ptr;
	};

	Data data;
	uint64_t length;
	uint64_t max_length = 7;

public:
	ChatString(wchar_t* string)
	{
		length = wcslen(string);
		while (max_length < length) max_length += 8;

		if (max_length >= MAX_STORE_LENGTH) data.str_ptr = new wchar_t[max_length + 1];

		wcscpy(GetData(), string);
	}

	~ChatString()
	{
		if (max_length >= MAX_STORE_LENGTH) delete[] data.str_ptr;
	}

	wchar_t* GetData()
	{
		return (max_length < MAX_STORE_LENGTH) ? data.str : data.str_ptr;
	}
};

class HookSendChat : public Hook
{
	static inline Hook* hook;

	static void SendChatMessage(void* self, wchar_t* text, int64_t colour)
	{
		auto message = ChatString(text);
		int proper_colour = (colour & 0xFF000000) | ((colour & 0xFF0000) >> 16) | (colour & 0xFF00) | ((colour & 0xFF) << 16);
		hook->Trampoline(SendChatHook)(self, message, proper_colour);
	}

#define COLOR_INFO 0x09D4E3
#define COLOR_GREEN 0x20E665
#define COLOR_RED 0xD44C4C

	static void HOOK SendChatHook(void* self, ChatString &message, int &colour)
	{
		std::wstring wstr(message.GetData());
		if (wstr.find(L"/pvp") != std::string::npos)
		{
			Main::GetInstance().TogglePvP();

			if (Main::GetInstance().CanPvP()) SendChatMessage(self, L"PvP Enabled.\n", COLOR_GREEN);
			else SendChatMessage(self, L"PvP Disabled.\n", COLOR_RED);
		}
		else if (wstr.find(L"/getcenter") != std::string::npos)
		{
			auto center_coordinates = Main::GetInstance().GetCenterCoordinates();
			wchar_t coords_str[1024];
			wsprintfW(coords_str, L"Center coordinates: %i, %i\n.", center_coordinates.x, center_coordinates.y);
			if (center_coordinates.x == INT_MIN && center_coordinates.y == INT_MIN) SendChatMessage(self, L"Center coordinates are not yet set.\n", COLOR_RED);
			else SendChatMessage(self, coords_str, COLOR_INFO);
		}
		else if (wstr.find(L"/setcenter") != std::string::npos)
		{
			auto player_coords = Main::GetInstance().GetLocalPlayer()->GetRegionCoordinates();

			if (Main::GetInstance().GetCenterCoordinates().x != INT_MIN || Main::GetInstance().GetCenterCoordinates().y != INT_MIN)
			{
				SendChatMessage(self, L"Center coordinates are already set for this character.\n", COLOR_RED);
			}
			else
			{
				Main::GetInstance().SetCenter(player_coords.x, player_coords.y);
				auto center_coordinates = Main::GetInstance().GetCenterCoordinates();

				wchar_t coords_str[1024];
				wsprintfW(coords_str, L"Set center coordinates: %i, %i.\n", center_coordinates.x, center_coordinates.y);
				SendChatMessage(self, coords_str, COLOR_GREEN);
			}
		}
		else hook->Trampoline(SendChatHook)(self, message, colour);
	}

public:
	HookSendChat() : Hook(MemoryHelper::GetCubeBase() + 0x26B670, (void*)SendChatHook, hook)
	{}
};