#pragma once

#include <cstdint>
#include <sstream>
#include <random>
#include <functional>
#include <main.h>
#include <climits>

struct RegionCoordinates
{
	int x = INT_MIN, y = INT_MIN;

	/**
	 * Returns the absolute distance between two region coordinates
	 * @param coordinates
	 * @return
	 */
	float operator-(RegionCoordinates coordinates)
	{
		float x_diff = abs(x - coordinates.x);
		float y_diff = abs(y - coordinates.y);
		return sqrt(x_diff * x_diff + y_diff * y_diff);
	}
};

enum ItemRarity
{
	COMMON, UNCOMMON, RARE, EPIC, LEGENDARY
};

enum Material
{
	STEEL = 4, OBSIDIAN = 5, BONE = 7, GOLD = 11, SILVER = 12, PLATINUM = 13, EMERALD = 14, RUBY = 16, DIAMOND = 17, SAURIAN = 19, ICE = 23, SHADOW = 40, DEMON = 42, DRUID = 44
};

enum WeaponType
{
	AXE = 1, MACE = 2, DAGGER = 3, FIST = 4, LONGSWORD = 5, BOW = 6, CROSSBOW = 7, BOOMERANG = 8, STAFF = 10, WAND = 11, BRACELET = 12, SHIELD = 13, GREATSWORD = 15, GREATAXE = 16, GREATMACE = 17
};

struct Item
{
	uint8_t GetItemType() //TODO: Make this an enum please
	{
		return *(uint8_t*)this;
	}

	bool IsEquipment()
	{
		return GetItemType() >= 3 && GetItemType() <= 9;
	}

	WeaponType GetWeaponSubtype()
	{
		return (WeaponType)(*(uint8_t*)(this + 0x4));
	}

	RegionCoordinates GetRegionCoordinates()
	{
		return *(RegionCoordinates*)(this + 0xC);
	}

	void SetRegionCoordinates(RegionCoordinates coordinates)
	{
		*(RegionCoordinates*)(this + 0xC) = coordinates;
	}

	ItemRarity GetRarity()
	{
		return (ItemRarity)(*(uint8_t*)(this + 0x14));
	}

	void SetRarity(ItemRarity rarity)
	{
		*(uint8_t*)(this + 0x14) = rarity;
	}

	void SetMaterial(Material material)
	{
		*(uint8_t*)(this + 0x1C) = material;
	}

	uint32_t GetModifier()
	{
		return *(uint32_t*)(this + 0x8);
	}

	void SetModifier(uint32_t modifier)
	{
		*(uint32_t*)(this + 0x8) = modifier;
	}

	void SetMythicWeaponMaterial()
	{
		//Pick a cool material I guess
		switch (GetWeaponSubtype())
		{
			case AXE:
			case MACE:
				SetMaterial(OBSIDIAN);
				break;

			case DAGGER:
			case BOW:
			case CROSSBOW:
			case BOOMERANG:
			case WAND:
				switch (rand() % 3)
				{
					case 0:
						SetMaterial(ICE);
						break;
					case 1:
						SetMaterial(DEMON);
						break;
					case 2:
						SetMaterial(SHADOW);
						break;
				}
				break;

			case BRACELET:
			case SHIELD:
				switch (rand() % 2)
				{
					case 0:
						SetMaterial(DIAMOND);
						break;
					case 1:
						SetMaterial(PLATINUM);
						break;
				}
				break;

			case GREATSWORD:
				switch (rand() % 3)
				{
					case 0:
						SetMaterial(DIAMOND);
						break;
					case 1:
						SetMaterial(EMERALD);
						break;
					case 2:
						SetMaterial(RUBY);
						break;
				}
				break;

			case GREATAXE:
				SetMaterial(SAURIAN);
				break;

			case GREATMACE:
				switch (rand() % 2)
				{
					case 0:
						SetMaterial(BONE);
						break;
					case 1:
						SetMaterial(SAURIAN);
						break;
				}
				break;

			case LONGSWORD:
				SetMaterial(OBSIDIAN);
				break;

			case STAFF:
				switch (rand() % 3)
				{
					case 0:
						SetMaterial(OBSIDIAN);
						break;
					case 1:
						SetMaterial(DRUID);
						break;
					case 2:
						SetMaterial(STEEL);
						break;
				}
				break;
		}
	}

	/**
	 * Just lower the spawn rates for good stuff
	 */
	void ModifyRarity(bool from_shop = false)
	{
		bool mythic = false;

		if (!from_shop)
		{
			static auto percentage_generator = std::bind(std::uniform_int_distribution<int>(1, 100), std::default_random_engine());
			int percentage = percentage_generator();
			int mythic_percentage = percentage_generator();
			int rarity_decrease = 0;

			switch (GetRarity())
			{
				case ItemRarity::UNCOMMON:
					if (percentage < 20) rarity_decrease = 1;
					break;

				case ItemRarity::RARE:
					if (percentage < 30) rarity_decrease = 1;
					break;

				case ItemRarity::EPIC:
					if (percentage < 40) rarity_decrease = 1;
					break;

				case ItemRarity::LEGENDARY:
					if (percentage < 15) rarity_decrease = 2;
					else if (percentage < 50) rarity_decrease = 1;
					else if (mythic_percentage > 90)
					{
						//Make the item mythic
						mythic = true;

						//Add a + to mythic items
						auto plus_delta = GetModifier() % 50;
						SetModifier(GetModifier() - plus_delta);

						if (GetItemType() == 3) SetMythicWeaponMaterial();
					}
					break;
			}

			SetRarity((ItemRarity)(GetRarity() - rarity_decrease));
		}

		if (!mythic)
		{
			//Remove + from non-mythic items
			auto plus_delta = GetModifier() % 10;
			if (plus_delta == 0) SetModifier(GetModifier() - 1);
		}
	}
};

enum EntityType
{
	PLAYER, HOSTILE, PASSIVE, NPC_0, NPC_1, UNK, PLAYER_PET, OBJECT
};

struct Entity
{
	EntityType GetEntityType()
	{
		return (EntityType)(*(uint8_t*)(this + 0x60));
	}

	RegionCoordinates GetRegionCoordinates()
	{
		return *(RegionCoordinates*)(this + 0x150);
	}

	float GetRegionMulitplier(Entity* entity, Item* item)
	{
		bool weapon = item->GetItemType() == 3;

		float weapon_multiplier = 0.f;
		float armor_multiplier = 0.f;

		//Move items if there's no center set
		auto center_coordinates = Main::GetInstance().GetCenterCoordinates();
		if (center_coordinates.x == INT_MIN && center_coordinates.y == INT_MIN)
		{
			//item->SetRegionCoordinates(center_coordinates);
			item->SetRegionCoordinates(RegionCoordinates{0, 0});
		}

		RegionCoordinates region_coordinates = ((Entity*)Main::GetInstance().GetLocalPlayer())->GetRegionCoordinates();

		switch (GetEntityType())
		{
			case EntityType::PLAYER:
			{
				region_coordinates = item->GetRegionCoordinates();
				weapon_multiplier = 0.045f;
				armor_multiplier = 0.05f;
				break;
			}

			case EntityType::HOSTILE:
			case EntityType::PASSIVE:
			case EntityType::UNK:
				weapon_multiplier = 0.14f;
				armor_multiplier = 0.2f;

				//Nerf wand enemies
				if (weapon && item->GetWeaponSubtype() == WAND || item->GetWeaponSubtype() == STAFF) weapon_multiplier *= 0.6f;
				break;

			case EntityType::NPC_0:
			case EntityType::NPC_1:
				weapon_multiplier = 0.06f;
				armor_multiplier = 0.07f;
				break;

			case EntityType::PLAYER_PET:
				weapon_multiplier = 0.04f;
				armor_multiplier = 0.06f;
				break;

			case EntityType::OBJECT:
				weapon_multiplier = 0.1f;
				armor_multiplier = 0.1f;
				break;
		}

		float region_difference = Main::GetInstance().GetDistanceFromCenter(region_coordinates);
		float mythic_modifier = 1.f;

		//Actually a mythic item (+ gear)
		if (GetEntityType() == PLAYER && item->GetRarity() == LEGENDARY && item->GetModifier() % 50 == 0)
		{
			//Boost the stats for mythic items here, 50% better to start with, decreasing by .1% per region down to 25%
			mythic_modifier = std::max(1.5f - region_difference * 0.001f, 1.25f);
		}

		return 1.f + (region_difference * (weapon ? weapon_multiplier : armor_multiplier)) * 2.5f * mythic_modifier;
	}
};

enum ArtifactType
{
	HEALTH, ARMOR, DAMAGE, HASTE, REGENERATION, CRIT, RESISTANCE
};

struct Player : public Entity
{
	std::string GetName()
	{
		return std::string((char*)this + 0x958);
	}

	Item** GetInventory()
	{
		return *(Item***)(this + 0x9F0);
	}

	/**
	 * Artifact stats are stored in the players' wing positions because they're networked and players don't have wings and this is easier
	 * @param artifact_type
	 * @return
	 */
	uint16_t GetNetworkedArtifactStats(ArtifactType artifact_type)
	{
		switch (artifact_type)
		{
			case HEALTH:
				return *(uint16_t*)(this + 0x11C);

			case ARMOR:
				return *(uint16_t*)(this + 0x11E);

			case RESISTANCE:
				return *(uint16_t*)(this + 0x120);

			default:
				//Some stuff doesn't need to be networked because the stats are calculated only by the local player (like damage)
				return 0;
		}
	}

	void UpdateNetworkedArtifactStats()
	{
		//Only 3 that need to be networked really
		*(uint16_t*)(this + 0x11C) = GetArtifactStats(HEALTH);
		*(uint16_t*)(this + 0x11E) = GetArtifactStats(ARMOR);
		*(uint16_t*)(this + 0x120) = GetArtifactStats(RESISTANCE);
	}

	int GetArtifactStats(ArtifactType artifact_type)
	{
		return *(int*)(this + 0xB5C + (4 * artifact_type));
	}

	void SetArtifactStats(ArtifactType artifact_type, int value)
	{
		*(int*)(this + 0xB5C + (4 * artifact_type)) = value;
	}

	//The values that the artifact base stats start at
	float GetArtifactTypeBaseStats(ArtifactType artifact_type)
	{
		switch (artifact_type)
		{
			case HEALTH:
				return 100.f;

			case ARMOR:
				return 0.f;

			case DAMAGE:
				return 5.f;

			case HASTE:
				return 0.f;

			case REGENERATION:
				return 100.f;

			case CRIT:
				return 0.f;

			case RESISTANCE:
				return 0.f;
		}

		return 0.f;
	}

	float GetIncreasedArtifactStats(ArtifactType artifact_type, bool for_display = false)
	{
		//Use the networked stats if we can, if we can't it's either actually 0 or it's not a networked stat
		auto artifact_amount = GetNetworkedArtifactStats(artifact_type);
		if (artifact_amount == 0) artifact_amount = GetArtifactStats(artifact_type);

		float stat_increase;
		switch (artifact_type)
		{
			case HEALTH:
				stat_increase = artifact_amount * 50.f;
				break;

			case ARMOR:
				stat_increase = artifact_amount * 2.0f;
				break;

			case DAMAGE:
				stat_increase = artifact_amount * 5.0f;
				break;

			case HASTE:
				stat_increase = artifact_amount * 0.075f;
				break;

			case REGENERATION:
				stat_increase = artifact_amount * 0.1f;
				break;

			case CRIT:
				stat_increase = artifact_amount * 0.035f;
				break;

			case RESISTANCE:
				stat_increase = artifact_amount * 2.0f;
				break;
		}

		if (for_display) stat_increase += GetArtifactTypeBaseStats(artifact_type);
		return stat_increase;
	}
};