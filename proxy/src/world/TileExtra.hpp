#pragma once
#include "../utils/Reader.hpp"
#include "../itemManager/items_dat_decode.h"
#include <cstdint>
#include <array>
#include <string>
#include <unordered_map>
#include <chrono>

#pragma pack (push, 1)
enum eTileType : std::uint8_t {
    NONE,
    DOOR,
    SIGN,
    LOCK,
    TREE,
    UNK_5,
    MAILBOX,
    BULLETIN,
    DICE,
    PROVIDER,
    ACHIEVEMENT_BLOCK,
    HEART_MONITOR,
    DONATION_BOX,
    TOY_BOX,
    MANNEQUIN,
    MAGIC_EGG,
    GAME_BLOCK,
    GAME_GENERATOR,
    XENONITE,
    DRESSUP,
    CRYSTAL,
    BURGLAR,
    SPOTLIGHT,
    DISPLAY_BLOCK,
    VENDING_MACHINE,
    FISHTANK,
    SOLAR,
    FORGE,
    GIVING_TREE,
    GIVING_TREE_STUMP,
    STEAM_ORGAN,
    SILKWORM,
    SEWING_MACHINE,
    FLAG,
    LOBSTER_TRAP,
    ART_CANVAS,
    BATTLE_CAGE,
    PET_TRAINER,
    STEAM_ENGINE,
    LOCK_BOT,
    BACKGROUND_WEATHER,
    SPIRIT_STORAGE,
    DATA_BEDROCK,
    DISPLAY_SHELF,
    VIP_TIMER,
    CHALLENGE_TIMER,
    UNK_46,
    FISH_MOUNT,
    PORTRAIT,
    STUFF_WEATHER,
    FOSSIL_PREP,
    DNA_MACHINE,
    TRICKSTER,
    CHEMTANK,
    STORAGE,
    OVEN,
    SUPER_MUSIC,
    GEIGER_CHARGER,
    ADVENTURE_RESET,
    TOMB_ROBBER,
    FACTION,
    TRAINING_FISH_PORT,
    ITEM_SUCKER,
    ROBOT,
    TICKET,
    GUILD_ITEM,
    STATS_BLOCK,
    FIELD_NODE,
    OUIJA_BOARD,
    AUTO_BREAK,
    AUTO_HARVEST,
    AUTO_HARVEST_SUCKER,
    LIGHTNING_IF_ON,
    PHASED_BLOCK,
    SAFE_VAULT,
    PHASED_BLOCK2,
    PVE_NPC,
    INFINITY_WEATHER,
    COMPLETIONIST,
    FEEDING_BLOCK,
    KRANKENS_BLOCK,
    FRIENDS_ENTRANCE,
    MAX
};

struct TileExtra {
    eTileType tileType;

    struct {
        std::string label;
        bool locked;
        uint8_t Unk1;
    } door;

    struct {
        std::string label;
        std::uint16_t label_len;
    } sign;

    struct {
        std::uint8_t unk;
        std::uint32_t owner_id;
        std::uint32_t access_size;
        std::vector<std::uint32_t> accesses;
    } lock;

    struct {
        uint8_t unk;
        uint32_t unk2;
        std::vector<std::uint32_t> unk3;

    } fishtank;

    struct {
        std::uint32_t seedTime;
        std::uint32_t plantTime;
        std::uint32_t readyTime;
        std::uint8_t fruit_count;
        bool spliced;

        inline std::time_t getCurrentTime() {
            auto currentTimePoint = std::chrono::system_clock::now();
            return std::chrono::system_clock::to_time_t(currentTimePoint);
        }
    } seed;

    struct {
        std::uint8_t number;
    } dice;

    struct {
        uint32_t unk;
        uint8_t unk2;
    } achievement;

    struct {
        uint32_t unk;
        uint32_t unk2;
    } provider;

    struct {
        std::uint16_t label_len;
        std::string label;
    } heart_monitor;

    struct {
        std::uint16_t label_len;
        std::string label;
        std::uint32_t hair_color;
        std::uint16_t cloth[9];

        uint8_t unk1;
        uint16_t unk2;
        uint16_t unk3;
        uint16_t hat;
        uint16_t shirt;
        uint16_t pants;
        uint16_t boots;
        uint16_t face;
        uint16_t hand;
        uint16_t back;
        uint16_t hair;
        uint16_t neck;

    } mannequin;

    struct {
        uint32_t unk;
    } bunny_egg;

    struct {
        uint8_t unk;
    } game_gen;

    struct {
        uint8_t unk;
        uint32_t unk2;
    } xenonite;

    struct {
        uint16_t len;
        std::string unk;
    } crystal;

    struct {
        uint16_t len;
        std::string unk;
        uint32_t unk2;
        uint8_t unk3;
    } burglar;

    struct {
        uint32_t item_id;
    } display_block;

    struct {
        uint32_t item_id;
        uint32_t price;
    } vending;

    struct {
        uint8_t unk;
        uint32_t unk2;
        std::vector<uint32_t> unk3;
    } solar;

    struct {
        uint8_t unk;
        uint32_t unk2;
        uint8_t unk3;
    } deco;

    struct {
        uint32_t unk;
    } sewing_machine;

    struct {
        uint16_t len;
        std::string flag;
    } country_flag;

    struct {
        uint16_t len;
        std::string label;
        uint32_t pet1;
        uint32_t pet2;
        uint32_t pet3;
    } battle_cage;

    struct {
        union {
            uint32_t color;
            uint32_t item_id;
        };
    } weather_special;

    struct {
        uint8_t unk;
        uint32_t owner_id;
        uint32_t access_size;
        std::vector<uint32_t> accesses;
    } vip_entrance;

    struct {
        uint32_t unk;
    } geiger_charger;

    struct {
        std::array<std::int32_t, 5> data;
    } data_bedrock;

    struct {
        //std::array<std::uint32_t, 4> item_id;
        uint32_t top_left;
        uint32_t top_right;
        uint32_t bottom_left;
        uint32_t bottom_right;
    } display_shelf;

    struct {
        uint16_t item1;
        uint16_t item2;
        uint16_t item3;
        string unkstring;
        uint32_t unk1;
        uint32_t unk2;
        uint32_t unk3;
        uint32_t unk4;
        uint32_t unk5;
        uint32_t unk6;
        uint32_t unk7;
    } portrait;



    struct { 
        uint8_t pattern_index;
        uint32_t unknown_1;
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } krankens_block;

    TileExtra()
        : tileType(NONE), door(), sign(), lock(), seed(), dice(), provider(), achievement(), heart_monitor(), bunny_egg()
        , game_gen(), xenonite(), crystal(), burglar(), display_block(), vending(), solar(), deco(), sewing_machine()
        , country_flag(), battle_cage(), weather_special(), vip_entrance(), geiger_charger(), fishtank(), mannequin(), data_bedrock(), display_shelf(), portrait(), krankens_block(){}
    ~TileExtra() = default;

    auto Serialize(void* buffer, std::size_t& position, std::uint16_t version, const std::pair<uint16_t, uint16_t>& tileArr) -> void {
        MemoryReader reader(buffer);
        reader.Skip(position);

        tileType = static_cast<eTileType>(reader.Read<std::uint8_t>());

        // Optional -> spdlog::info("Serializing {} ({})", TypeToString(tileType), tileArr.first);
        std::cout << "\n\nSerializing: " + TypeToString(tileType) + " | Item Id: " + to_string(tileArr.first) + "\n\n";

        switch (tileType) {
        case eTileType::DOOR:
            door.label = reader.ReadString();
            door.Unk1 = reader.Read<uint8_t>();
            break;
        case eTileType::SIGN:
            sign.label = reader.ReadString();
            sign.label_len = reader.Read<uint32_t>();
            break;
        case eTileType::LOCK:

            lock.unk = reader.Read<uint8_t>();
            lock.owner_id = reader.Read<uint32_t>();
            lock.access_size = reader.Read<uint32_t>();

            lock.accesses.reserve(lock.access_size);

            for (uint32_t i = 0; i < lock.access_size; i++) {
                //if (i >= 7) break; //
                lock.accesses.push_back(reader.Read<uint32_t>());
            }

            reader.Skip(8);

            if (tileArr.first == 5814) // Guild Lock
                reader.Skip(16);
            break;
        case eTileType::TREE:
            seed.readyTime = reader.Read<uint32_t>();
            seed.fruit_count = reader.Read<uint8_t>();
            break;
        case eTileType::DICE:
            dice.number = reader.Read<uint8_t>();
            break;
        case eTileType::PROVIDER:
            provider.unk = reader.Read<uint32_t>();

            if (tileArr.first != 5318 && (tileArr.first != 10656 || version < 17))
                break;

            provider.unk2 = reader.Read<uint32_t>();
            break;
        case eTileType::ACHIEVEMENT_BLOCK:
            achievement.unk = reader.Read<uint32_t>();
            achievement.unk2 = reader.Read<uint8_t>();
            break;
        case eTileType::HEART_MONITOR:
            heart_monitor.label_len = reader.Read<uint32_t>();
            heart_monitor.label = reader.ReadString();
            break;
        case eTileType::MANNEQUIN: 
            mannequin.label = reader.ReadString();
            mannequin.unk1 = reader.Read<uint8_t>();
            mannequin.unk2 = reader.Read<uint16_t>();
            mannequin.unk3 = reader.Read<uint16_t>();
            mannequin.hat = reader.Read<uint16_t>();
            mannequin.shirt = reader.Read<uint16_t>();
            mannequin.pants = reader.Read<uint16_t>();
            mannequin.boots = reader.Read<uint16_t>();
            mannequin.face = reader.Read<uint16_t>();
            mannequin.hand = reader.Read<uint16_t>();
            mannequin.back = reader.Read<uint16_t>();
            mannequin.hair = reader.Read<uint16_t>();
            if (mannequin.hair == 5712 || mannequin.boots == 10044) {
                reader.Skip(8);
            }
            mannequin.neck = reader.Read<uint16_t>();
            break;
        case eTileType::MAGIC_EGG:
            bunny_egg.unk = reader.Read<uint32_t>();
            break;
        case eTileType::GAME_GENERATOR: break; //remove read
        case eTileType::XENONITE:
            xenonite.unk = reader.Read<uint8_t>();
            xenonite.unk2 = reader.Read<uint32_t>();
            break;
        case eTileType::DRESSUP:
            reader.Skip(18);
            break;
        case eTileType::CRYSTAL:
            crystal.unk = reader.ReadString();
            break;
        case eTileType::BURGLAR:
            burglar.unk = reader.ReadString();
            burglar.unk2 = reader.Read<uint32_t>();
            burglar.unk3 = reader.Read<uint8_t>();
            break;
        case eTileType::SPOTLIGHT: break;
        case eTileType::DISPLAY_BLOCK:
            display_block.item_id = reader.Read<uint32_t>();
            break;
        case eTileType::VENDING_MACHINE:
            vending.item_id = reader.Read<uint32_t>();
            vending.price = reader.Read<uint32_t>();
            if ((int)vending.price < 0)
                vending.price = std::abs(static_cast<int>(vending.price));
            break;
        case eTileType::FISHTANK:
            //reader.SkipUntil();
            //reader.Skip(reader.Read<std::uint32_t>() * 4);

            fishtank.unk = reader.Read<std::uint8_t>();
            fishtank.unk2 = reader.Read<std::uint32_t>();
            for (uint32_t i = 0; i < fishtank.unk2; i++) {
                fishtank.unk3.push_back(reader.Read<std::uint32_t>());
            }
            break;
        case eTileType::SOLAR:
            solar.unk = reader.Read<uint8_t>();
            solar.unk2 = reader.Read<uint32_t>();
            for (uint32_t i = 0; i < solar.unk2; i++) {
                solar.unk3.push_back(reader.Read<uint32_t>());
            }
            break;
        case eTileType::FORGE:
            reader.Skip(4);
            break;
        case eTileType::GIVING_TREE:
            deco.unk = reader.Read<uint8_t>();
            deco.unk2 = reader.Read<uint32_t>();
            deco.unk3 = reader.Read<uint8_t>();
            break;
        case eTileType::STEAM_ORGAN:
            reader.Skip(5);
            break;
        case eTileType::SILKWORM:
            reader.Skip(1);
            reader.SkipString();
            reader.Skip(4);
            reader.Skip(4);
            reader.Skip(4);
            reader.Skip(1);
            reader.Skip(4);
            reader.Skip(4);
            reader.Skip(4);
            reader.Skip(4);

            break;
        case eTileType::SEWING_MACHINE:
            deco.unk3 = reader.Read<uint32_t>();
            break;
        case eTileType::FLAG:
            if (tileArr.first == 3394)
                country_flag.flag = reader.ReadString();
            break;
        case eTileType::ART_CANVAS:
            reader.Skip(4);
            reader.SkipString();
            break;
        case eTileType::BATTLE_CAGE:
            battle_cage.label = reader.ReadString();
            battle_cage.pet1 = reader.Read<uint32_t>();
            battle_cage.pet2 = reader.Read<uint32_t>();
            battle_cage.pet3 = reader.Read<uint32_t>();
            break;
        case eTileType::LOCK_BOT:
            reader.Skip(4);
            break;
        case eTileType::BACKGROUND_WEATHER:
            weather_special.color = reader.Read<uint32_t>();
            break;
        case eTileType::DATA_BEDROCK:
            reader.Skip(21);
            break;
        case eTileType::DISPLAY_SHELF: //new
            //reader.Skip(16);
            display_shelf.top_left = reader.Read<uint32_t>();
            display_shelf.top_right = reader.Read<uint32_t>();
            display_shelf.bottom_left = reader.Read<uint32_t>();
            display_shelf.bottom_right = reader.Read<uint32_t>();
            break;
        case eTileType::VIP_TIMER:
            vip_entrance.unk = reader.Read<uint8_t>();
            vip_entrance.owner_id = reader.Read<uint32_t>();
            vip_entrance.access_size = reader.Read<uint32_t>();

            vip_entrance.accesses.reserve(vip_entrance.access_size);

            for (uint32_t i = 0; i < vip_entrance.access_size; i++) {
                vip_entrance.accesses.push_back(reader.Read<uint32_t>());
            }
            break;
        case eTileType::CHALLENGE_TIMER: break; //new
        case eTileType::FISH_MOUNT:
            reader.SkipString();
            reader.Skip(5);
            break;
        case eTileType::PORTRAIT:
            reader.SkipString();
            reader.Skip(16);
            portrait.item1 = reader.Read<uint16_t>();
            portrait.item2 = reader.Read<uint16_t>();
            portrait.item2 = reader.Read<uint16_t>();
            if (portrait.item1 == 5712 || portrait.item1 == 10044 || portrait.item2 == 5712 || portrait.item2 == 10044 || portrait.item3 == 5712 || portrait.item3 == 10044) {
                reader.Skip(8);
            }
            reader.Skip(4);
            if (portrait.item2 == 12958)
                reader.SkipString();
            break;
        case eTileType::STUFF_WEATHER:
            reader.Skip(9);
            break;
        case eTileType::STORAGE:
            reader.SkipString();
            break;
        case eTileType::OVEN:
            reader.Skip(reader.Read<uint32_t>() * 4);
            reader.Skip(16);
            break;
        case eTileType::SUPER_MUSIC:
            reader.SkipString();
            reader.Skip(4);
            break;
        case eTileType::GEIGER_CHARGER:
            geiger_charger.unk = reader.Read<uint32_t>();
            break;
        case eTileType::ADVENTURE_RESET:
            break;
        case eTileType::TRAINING_FISH_PORT:
            reader.Skip(35);
            break;
        case eTileType::ITEM_SUCKER:
            reader.Skip(14);
            break;
        case eTileType::ROBOT:
            reader.Skip(reader.Read<uint32_t>() * 15);
            reader.Skip(8);
            break;
        case eTileType::GUILD_ITEM:
            reader.Skip(17);
            break;
        case eTileType::STATS_BLOCK:
            reader.SkipUntil();
            break;
        case eTileType::AUTO_BREAK:
            reader.Skip(16);
            break;
        case eTileType::AUTO_HARVEST_SUCKER:
            reader.Skip(44);
            break;
        case eTileType::LIGHTNING_IF_ON:
            reader.Skip(12);
            break;
        case eTileType::PHASED_BLOCK:
            reader.Skip(4);
            break;
        case eTileType::SAFE_VAULT:
            break;
        case eTileType::PHASED_BLOCK2:
            reader.Skip(4);
            reader.SkipString();
            break;
        case eTileType::INFINITY_WEATHER:
            reader.Skip(4);
            reader.Skip(reader.Read<uint32_t>() * 4);
            break;
        case eTileType::KRANKENS_BLOCK: //new and test only
            krankens_block.pattern_index = reader.Read<uint8_t>();
            krankens_block.unknown_1 = reader.Read<uint32_t>();
            krankens_block.r = reader.Read<uint8_t>();
            krankens_block.g = reader.Read<uint8_t>();
            krankens_block.b = reader.Read<uint8_t>();
            break;
        case eTileType::FRIENDS_ENTRANCE:
            reader.Skip(8);
            break;

        default:
            std::cout << "\n------------------ failed to serialize: " + TypeToString(tileType) + " Item Id: " + to_string(tileArr.first) + "\n";
            break;

        }



        position = reader.GetPosition();
    }

    inline std::string TypeToString(eTileType index) const {
        std::array<std::string, eTileType::MAX> types {
            "None", "Door", "Sign", "Lock", "Tree", "Unk_5", "Mailbox", "Bulletin", "Dice", "Provider",
            "Achievement_Block", "Heart_Monitor", "Donation_Box", "Toy_Box", "Mannequin", "Magic_Egg", "Game_Block", "Game_Generator", "Xenonite", "Dressup",
            "Crystal", "Burglar", "Spotlight", "Display_Block", "Vending_Machine", "Fishtank", "Solar", "Forge", "Giving_Tree", "Giving_Tree_Stump", "Steam_Organ", "Silkworm",
            "Sewing_Machine", "Flag", "Lobster_Trap", "Art_Canvas", "Battle_Cage", "Pet_Trainer", "Steam_Engine", "Lock_Bot", "Background_Weather", "Spirit_Storage", "Data_Bedrock", "Display_Shelf",
            "Vip_Timer", "Challenge_Timer", "Unk_46", "Fish_Mount", "Portrait", "Stuff_Weather", "Fossil_Prep", "Dna_Machine", "Trickster", "Chemtank", "Storage", "Oven",
            "Super_Music", "Geiger_Charger", "Adventure_Reset", "Tomb_Robber", "Faction", "Training_Fish_Port", "Item_Sucker", "Robot", "Ticket", "Guild_Item", "Stats_Block", "Field_Node",
            "Ouija_Board", "Auto_Break", "Auto_Harvest", "Auto_Harvest_Sucker", "Lightning_If_On", "Phased_Block", "Safe_Vault", "Phased_Block2", "Pve_Npc", "Infinity_Weather", "Completionist", "Feeding_Block",
            "Krankens_Block", "Friends_Entrance"
        };

        if (index > MAX || index < NONE)
            index = NONE;

        return types[index];
    }
};
#pragma pack(pop)