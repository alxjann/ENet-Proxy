#pragma once
#include "TileExtra.hpp"
#include "../utils/Math.hpp"
#include "../utils/Reader.hpp"
#include <cstdint>
#include <deque>

#pragma pack (push, 1)
struct WorldTile {
    enum eTileFlags : std::uint16_t {
        NONE = 0 << 0,
        EXTRA_TILE = 1 << 0,
        LOCKED = 1 << 1,
        SPLICED_TREE = 1 << 2,
        TREE_WILL_DROP_SEED = 1 << 3,
        TREE = 1 << 4,
        FLIPPED = 1 << 5,
        ENABLED = 1 << 6,
        PUBLIC = 1 << 7,
        EXTRA_FRAME = 1 << 8,
        SILENCED = 1 << 9,
        WATER = 1 << 10,
        GLUE = 1 << 11,
        FIRE = 1 << 12,
        RED = 1 << 13,
        GREEN = 1 << 14,
        BLUE = 1 << 15,
        MAX = 15,
    };

    std::uint16_t foreground;
    std::uint16_t background;
    std::uint16_t parentTile;
    std::uint16_t lockParentTile;

    eTileFlags flag;
    TileExtra tileExtra;
    CL_Vec2F position;

    WorldTile() : foreground(0), background(0), parentTile(0), lockParentTile(0), flag(eTileFlags::NONE), tileExtra(), position({}) {}
    ~WorldTile() = default;


    auto Serialize(void* buffer, std::uint16_t version) -> void {
        std::size_t temp{ 0 };
        Serialize(buffer, temp, version);
    }    
    
    auto Serialize(void* buffer, std::size_t& position, std::uint16_t version) -> void {
        MemoryReader reader(buffer);
        reader.Skip(position);

        foreground = reader.Read<std::uint16_t>();
        background = reader.Read<std::uint16_t>();
        parentTile = reader.Read<std::uint16_t>();
        flag = static_cast<eTileFlags>(reader.Read<std::uint16_t>());

        //if (parentTile)
        if ((flag & eTileFlags::LOCKED) == eTileFlags::LOCKED)
            lockParentTile = reader.Read<std::uint16_t>();
            

        position = reader.GetPosition();

        if ((flag & eTileFlags::EXTRA_TILE) == eTileFlags::EXTRA_TILE) {
            tileExtra.Serialize(buffer, position, version, std::make_pair(foreground, background));
        }
    }

    const char* FlagToString(eTileFlags flag) const {
        switch (flag) {
        case eTileFlags::NONE: return "NONE";
        case eTileFlags::EXTRA_TILE: return "EXTRA";
        case eTileFlags::LOCKED: return "LOCKED";
        case eTileFlags::SPLICED_TREE: return "SPLICED_TREE";
        case eTileFlags::TREE_WILL_DROP_SEED: return "TREE_WILL_DROP_SEED";
        case eTileFlags::TREE: return "SEED";
        case eTileFlags::FLIPPED: return "FLIPPED";
        case eTileFlags::ENABLED: return "OPEN";
        case eTileFlags::PUBLIC: return "PUBLIC";
        case eTileFlags::EXTRA_FRAME: return "EXTRA_FRAME";
        case eTileFlags::SILENCED: return "SILENCED";
        case eTileFlags::WATER: return "WATER";
        case eTileFlags::GLUE: return "GLUE";
        case eTileFlags::FIRE: return "FIRE";
        case eTileFlags::RED: return "RED";
        case eTileFlags::GREEN: return "GREEN";
        case eTileFlags::BLUE: return "BLUE";
        default: return "Unknown";
        }
    }
};

class WorldTileMap {
public:
    CL_Vec2<std::int32_t> worldSize;
    std::vector<WorldTile> tiles;
    std::uint32_t itemCount;
    //std::unordered_map<uint32_t, WorldTile> tiles2;

    WorldTileMap() : worldSize({}), tiles(), itemCount(0) {}
    ~WorldTileMap() {
        tiles.clear();
    }

    auto Serialize(std::uint8_t* buffer, std::size_t& position, std::uint16_t version) -> void {
        MemoryReader reader(buffer);
        reader.Skip(position);

        worldSize = reader.Read<CL_Vec2<std::int32_t>>();
        itemCount = reader.Read<std::uint32_t>();
        
        tiles.reserve(itemCount);

        reader.Skip(5);

        position = reader.GetPosition();
        

        for (std::uint32_t i = 0; i < itemCount; i++) {
            WorldTile tile{};
            tile.Serialize(buffer, position, version);

            tile.position.X = i % worldSize.X;
            tile.position.Y = i / worldSize.X;

            tiles.push_back(tile);
        }



/*
        for (int y = 0; y < worldSize.Y; y++) {
            for (int x = 0; x < worldSize.X; x++) {
                WorldTile tile{};
                tile.Serialize(buffer, position, version);
                //cout << "\nindex: " << to_string(i) << "\nworldSize Y: " << worldSize.Y;
                tile.position.X = x;
                tile.position.Y = y;

                cout << "\nTile ID: " << tile.foreground << "\nTile PosX: " << tile.position.X << "\nTile PosY: " << tile.position.Y;
                tiles.push_back(tile);
            }
        }
*/
    }

    WorldTile* GetTileAtIndex(std::int32_t x, std::int32_t y) noexcept {
       // if (x >= 0 && y >= 0 && worldSize.X > x && worldSize.Y > y)
            return &tiles[static_cast<std::size_t>(y * worldSize.X + x)];

        //return nullptr;
    }
};
#pragma pack(pop)