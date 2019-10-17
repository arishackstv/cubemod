#pragma once

#include <map>
#include <Windows.h>
#include <vector>


class Player;

class RegionCoordinates;

class Main
{
private:
	std::map<std::string, RegionCoordinates> center_coordinates;

	bool pvp_enabled;

public:
	HMODULE module;

private:
	void SetConsole(bool open);

	void SetupHooks();

	void ChangeArtifactDisplay();

	void PatchRegionLock();

public:
	static Main& GetInstance();

	void Start();

	Player* GetLocalPlayer();

	void TogglePvP();

	bool CanPvP()
	{
		return pvp_enabled;
	}

	void SetCenter(int x, int y);

	RegionCoordinates GetCenterCoordinates();

	float GetDistanceFromCenter(RegionCoordinates region);

	void LoadConfig();

	void SaveConfig();
};




































