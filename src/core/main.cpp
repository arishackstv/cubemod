#include "main.h"

#include <fstream>

#include "memory/memory_helper.h"

//Hook stuff wheee
#include "hook/hooks/item_generation/hook_create_weapon.h"
#include "hook/hooks/item_generation/hook_create_armor.h"
#include "hook/hooks/item_generation/hook_give_item.h"
#include "hook/hooks/scaling/hook_get_item_modifier.h"
#include "hook/hooks/scaling/hook_get_item_regen.h"
#include "hook/hooks/scaling/hook_get_item_haste.h"
#include "hook/hooks/scaling/hook_get_item_crit.h"
#include "hook/hooks/pvp/hook_can_attack_entity.h"
#include "hook/hooks/artifact/hook_set_artifact_stats.h"
#include "hook/hooks/artifact/display/hook_concat_artifact_suffix.h"
#include "hook/hooks/artifact/display/hook_round_artifact_thing.h"
#include "hook/hooks/artifact/display/hook_artifact_display_roundf.h"
#include "hook/hooks/artifact/base_stats/hook_get_hit_points.h"
#include "hook/hooks/artifact/base_stats/hook_get_player_armor.h"
#include "hook/hooks/artifact/base_stats/hook_get_attack_power.h"
#include "hook/hooks/artifact/base_stats/hook_get_spell_power.h"
#include "hook/hooks/artifact/base_stats/hook_get_player_haste.h"
#include "hook/hooks/artifact/base_stats/hook_get_player_regeneration.h"
#include "hook/hooks/artifact/base_stats/hook_get_player_resistance.h"
#include "hook/hooks/artifact/base_stats/hook_get_player_crit.h"
#include "hook/hooks/command/hook_send_chat.h"

Main& Main::GetInstance()
{
	static auto instance = Main();
	return instance;
}

void Main::Start()
{
	//Config :D
	LoadConfig();

	//Very fun
	PatchRegionLock();
	ChangeArtifactDisplay();

	SetupHooks();
}


//TODO: Maybe don't use hardcoded offsets but it's not like the game will ever update anyway so it's fine
void Main::SetupHooks()
{
	//Used to change spawn rates
	new HookCreateWeapon();
	new HookCreateArmor();

	//Intercept crafting here to change spawn rates
	new HookGiveItem();

	//Rebalancing items a bit
	new HookGetItemModifier();
	new HookGetItemRegen();
	new HookGetItemHaste();
	new HookGetItemCrit();

	//Ooh pvp
	new HookIsEnemy();

	//Used for updating networked artifact stats
	new HookSetArtifactStats();

	//Artifact display stuff
	new HookConcatArtifactSuffix();
	new HookRoundArtifactThing();
	new Hookroundf();

	//Artifact value stuff
	new HookGetMaxHitPoints();
	new HookGetPlayerArmor();
	new HookGetAttackPower();
	new HookGetSpellPower();
	new HookGetPlayerHaste();
	new HookGetPlayerRegeneration();
	new HookGetPlayerResistance();
	new HookGetPlayerCrit();

	//Command stuff
	new HookSendChat();
}

//TODO: You could replace these with hooks pretty easily, should probably do that
void Main::ChangeArtifactDisplay()
{
	//Item display
	MemoryHelper::FindAndReplaceString(L"Increases hang gliding speed.", L"Increases base regeneration.");
	MemoryHelper::FindAndReplaceString(L"You can climb faster.", L"Increases base health.");
	MemoryHelper::FindAndReplaceString(L"Increases sailing speed.", L"Increases base crit.");
	MemoryHelper::FindAndReplaceString(L"Increases swimming speed.", L"Increases base armor.");
	MemoryHelper::FindAndReplaceString(L"You consume less stamina when diving.", L"Increases base damage.");
	MemoryHelper::FindAndReplaceString(L"Increases riding speed.", L"Increases base haste.");
	MemoryHelper::FindAndReplaceString(L"Increases the radius of your lamp.", L"Increases base resistance.");
	
	//Inventory display
	MemoryHelper::FindAndReplaceString(L"Climbing Speed", L"Base Health");
	MemoryHelper::FindAndReplaceString(L"Swimming Speed", L"Base Armor");
	MemoryHelper::FindAndReplaceString(L"Diving Skill", L"Base Damage");
	MemoryHelper::FindAndReplaceString(L"Riding Speed", L"Base Haste");
	MemoryHelper::FindAndReplaceString(L"Hang Gliding Speed", L"Base Regen");
	MemoryHelper::FindAndReplaceString(L"Sailing Speed", L"Base crit");
	MemoryHelper::FindAndReplaceString(L"Light Radius", L"Base Resistance");
}

//Stole this all from Geordan9 at https://fearlessrevolution.com/viewtopic.php?p=104856#p104856 very nice
void Main::PatchRegionLock()
{
	//Might take a second
	uint64_t item_1;
	while (!(item_1 = MemoryHelper::FindPattern("08 ? ? ? ? 39 01 0F 85"))) Sleep(250);

	auto item_2 = MemoryHelper::FindPattern("39 01 0F 85 ? ? ? ? 48 FF");
	auto item_3 = MemoryHelper::FindPattern("41 39 42 ? 0F 85 ? ? ? ? 41");
	auto item_4 = MemoryHelper::FindPattern("0A ? ? ? ? 39 01 0F 85");
	auto item_5 = MemoryHelper::FindPattern("41 39 00 75 ? 48 FF C1");

	MemoryHelper::PatchMemory<uint8_t>(item_1 + 0x8, 0x80);
	MemoryHelper::PatchMemory<uint8_t>(item_2 + 0x3, 0x80);
	MemoryHelper::PatchMemory<uint8_t>(item_3 + 0x5, 0x80);
	MemoryHelper::PatchMemory<uint8_t>(item_4 + 0x8, 0x80);
	MemoryHelper::PatchMemory<uint8_t>(item_5 + 0x3, 0x70);
}

void Main::SetConsole(bool open)
{
	if (open)
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
	}
	else
	{
		fclose(stdout);
		FreeConsole();
	}
}

float Main::GetDistanceFromCenter(RegionCoordinates region)
{
	if (region.x == INT_MIN && region.y == INT_MIN) return 0.f;
	if (GetCenterCoordinates().x == INT_MIN && GetCenterCoordinates().y == INT_MIN) return 0.f;
	return region - GetCenterCoordinates();
}

Player* Main::GetLocalPlayer()
{
	auto game_base = *(uint64_t*)(MemoryHelper::GetCubeBase() + 0x551A80);
	return *(Player**)(*(uint64_t*)(game_base + 0x8) + 0x448);
}

void Main::SetCenter(int x, int y)
{
	center_coordinates[GetLocalPlayer()->GetName()] = RegionCoordinates{x, y};
	SaveConfig();
}

void Main::TogglePvP()
{
	pvp_enabled = !pvp_enabled;
	SaveConfig();
}

RegionCoordinates Main::GetCenterCoordinates()
{
	return center_coordinates[GetLocalPlayer()->GetName()];
}

#define CONFIG_PATH "modcfg.txt"

static std::vector<std::string> Split(std::string s, std::string delimiter)
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
	{
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

void Main::LoadConfig()
{
	auto in = std::fstream(CONFIG_PATH, std::fstream::in);
	for (std::string line; std::getline(in, line);)
	{
		auto character_config = Split(line, "|");
		if (character_config.size() == 1)
		{
			//First line has pvp toggle
			pvp_enabled = stoi(character_config[0]);
		}
		else if (character_config.size() == 3)
		{
			//Other lines have center coords
			auto character_name = character_config[0];
			auto region_coords = RegionCoordinates{stoi(character_config[1]), stoi(character_config[2])};
			center_coordinates[character_name] = region_coords;
		}
	}
}

void Main::SaveConfig()
{
	auto out = std::fstream(CONFIG_PATH, std::fstream::out);

	out << pvp_enabled << std::endl;

	for (auto entry : center_coordinates)
	{
		out << entry.first << "|" << entry.second.x << "|" << entry.second.y << std::endl;
	}
}
