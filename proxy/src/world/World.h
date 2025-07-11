#pragma once
#include "WorldTileMap.hpp"
#include "WorldObjectMap.hpp"
#include "../utils/Reader.hpp"
#include "../player/player.h"
#include "../packet/packet.h"
//#include "../Core/SDK/Game/Player/NetAvatar.hpp"
//#include <spdlog/fmt/bin_to_hex.h>
#include <string_view>
#include "../game/gt.hpp"


#pragma pack (push, 1)
class World {
public:
	World() : m_nameLen(0), m_Name(), m_Version(0), m_Flags(0) {};
	bool connected{};
	auto& GetName() { return m_Name; }
	auto& GetVersion() { return m_Version; }
	auto& GetFlags() { return m_Flags; }

	auto& GetTileMap() { return m_tileMap; }
	auto& GetObjectMap() { return m_objectMap; }
	auto& GetPlayers() { return players; }
	auto& GetLocalPlayer() { return local_player; }
	auto Serialize(std::uint8_t* buffer, gameupdatepacket_t* packet) -> void {
		m_tileMap.tiles.clear();
		m_objectMap.objects.clear();
		players.clear();
		//this->ResetData();

		MemoryReader reader(buffer);

		m_Version = reader.Read<std::uint16_t>();
		m_Flags = reader.Read<std::uint32_t>();

		m_nameLen = reader.Read<std::uint16_t>();
		reader.BackToPos(sizeof(std::uint16_t));
		m_Name = reader.ReadString();

		std::size_t position = reader.GetPosition();

		GetTileMap().Serialize(buffer, position, GetVersion());
		GetObjectMap().Serialize(buffer, position);
		
		reader.SetPosition(position);

		//m_Weather = reader.Read<std::uint32_t>();
		m_Weather = reader.Read<std::uint16_t>();
		reader.Skip(2);
		//m_baseWeather = reader.Read<std::uint32_t>();
		m_baseWeather = reader.Read<std::uint16_t>();
		reader.Skip(6);
		connected = true;
	}

	auto ResetData() -> void {
		m_tileMap.tiles.clear();
		m_objectMap.objects.clear();

		players.clear();

		m_Name = "EXIT";
		m_nameLen = 0;
		m_Version = 0;
		m_Flags = 0;
		m_Weather = 0;
		m_baseWeather = 0;

		m_tileMap.worldSize = {};
		m_tileMap.itemCount = -1;
		m_objectMap.objectCount = -1;
	}

private:
	std::uint16_t m_nameLen;
	std::string m_Name{};
	std::uint16_t m_Version;
	std::uint32_t m_Flags;
	std::uint32_t m_Weather;
	std::uint32_t m_baseWeather;

private:
	WorldTileMap m_tileMap;
	WorldObjectMap m_objectMap;
	std::vector<player> players{};
	player local_player{};
};
#pragma pack(pop)