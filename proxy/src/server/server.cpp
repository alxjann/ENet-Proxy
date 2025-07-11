#include "server.h"
#include <iostream>
#include "../events/events.h"
#include "../game/gt.hpp"
#include "../../vendor/proton/hash.hpp"
#include "../../vendor/proton/rtparam.hpp"
#include "../utils/utils.h"
#include <chrono>
#include "../itemManager/items_dat_decode.h"
#include "../utils/print/print.h"
#include "../world/World.h"
#include "../player/struct.h"
#include "../utils/Reader.hpp"
#include "../pathfinder/Pathfinder2.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../network/httplib.h"

//using namespace std;

static std::chrono::steady_clock::time_point lastGemSpawnTime;

bool hasAccessName(string nameAccess) {
    if (nameAccess.find("``") != -1 && (nameAccess.find("`^") != -1 || nameAccess.find("`2") != -1)) {
        return true;
    }
    else {
        return false;
    }
}

int getState() {
    int val = 0;
    val |= gt::noclip ? 1 : 2;
    return val;
}

int getState2() {
    int val = 0;
    val |= gt::noclip2 ? 1 : 2;
    val |= gt::doublejump ? 1 : 1;
    return val;
}


uint64_t pingsum = 0;
uint64_t total_ping_count = 0;
int average_ping;


int server::GetPing() {

    if (m_server_peer && m_server_peer->roundTripTime != 500) {
        // Save the old sum
        uint64_t old_sum = pingsum;

        // Add the current round trip time to the sum
        pingsum += m_server_peer->roundTripTime;

        // Increment the ping count
        total_ping_count += 1;

        // Check for overflow
        if (old_sum > pingsum) {
            // Overflow occurred, reset the sum and count
            pingsum = m_peer->roundTripTime;
            total_ping_count = 1;
        }
        average_ping = (total_ping_count > 0) ? pingsum / total_ping_count : 0;
        return average_ping;
    }

    /*    // Calculate and return the average ping
        int average_ping = (total_ping_count > 0) ? static_cast<int>(pingsum / total_ping_count) : 0;
        return average_ping;

        if (m_server_peer) {
            // Get the ping of the server peer

            enet_uint32 ping = m_server_peer->roundTripTime;

            // Print the ping
            PRINTS("Server ping: %u ms\n", ping);

            // Return the ping value
            return static_cast<int>(ping);
        }
        else {
            // Handle the case where the server peer is not available
            PRINTS("Server peer not available. No ping information.\n");
        }*/
}

struct PlayerMovings {
    int packetType;
    int netID;
    float x;
    float y;
    int characterState;
    int plantingTree;
    float XSpeed;
    float YSpeed;
    int punchX;
    int punchY;
    int SecondaryNetID;
};
bool collecting = true;

BYTE* packPlayerMovings(PlayerMovings* dataStruct) {
    BYTE* data = new BYTE[56];
    for (int i = 0; i < 56; i++) {
        data[i] = 0;
    }
    memcpy(data, &dataStruct->packetType, 4);
    memcpy(data + 4, &dataStruct->netID, 4);
    memcpy(data + 8, &dataStruct->SecondaryNetID, 4);
    memcpy(data + 12, &dataStruct->characterState, 4);
    memcpy(data + 20, &dataStruct->plantingTree, 4);
    memcpy(data + 24, &dataStruct->x, 4);
    memcpy(data + 28, &dataStruct->y, 4);
    memcpy(data + 32, &dataStruct->XSpeed, 4);
    memcpy(data + 36, &dataStruct->YSpeed, 4);
    memcpy(data + 44, &dataStruct->punchX, 4);
    memcpy(data + 48, &dataStruct->punchY, 4);
    return data;
}

PlayerMovings* unpackRaw(BYTE* data) {
    PlayerMovings* p = new PlayerMovings;
    memcpy(&p->packetType, data, 4);
    memcpy(&p->netID, data + 4, 4);
    memcpy(&p->characterState, data + 12, 4);
    memcpy(&p->plantingTree, data + 20, 4);
    memcpy(&p->x, data + 24, 4);
    memcpy(&p->y, data + 28, 4);
    memcpy(&p->XSpeed, data + 32, 4);
    memcpy(&p->YSpeed, data + 36, 4);
    memcpy(&p->punchX, data + 44, 4);
    memcpy(&p->punchY, data + 48, 4);
    return p;
}
void server::NoclipState(int netid) {
    PlayerMovings data;
    data.packetType = 0x14;
    data.characterState = 0; // animation
    data.x = 1000;
    data.y = 100;
    data.punchX = 500;
    data.punchY = 0;
    data.XSpeed = 2000;
    data.YSpeed = 500;
    data.netID = netid;
    data.plantingTree = getState();
    BYTE* raw = packPlayerMovings(&data);
    int var = 0x808000;
    float water = 125.0f;
    memcpy(raw + 1, &var, 3);
    memcpy(raw + 16, &water, 4);
    g_server->SendPacketRaw(true, 4, raw, 56, 0, ENET_PACKET_FLAG_RELIABLE);
}


void server::NoclipState2(int netid) {
    PlayerMovings data;
    data.packetType = 0x14;
    data.characterState = 0; // animation
    data.x = 1000;
    data.y = 100;
    data.punchX = 500;
    data.punchY = 0;
    data.XSpeed = 2000;
    data.YSpeed = 500;
    data.netID = netid;
    data.plantingTree = getState2();
    BYTE* raw = packPlayerMovings(&data);
    int var = 0x808000;
    float water = 125.0f;
    memcpy(raw + 1, &var, 3);
    memcpy(raw + 16, &water, 4);
    g_server->SendPacketRaw(true, 4, raw, 56, 0, ENET_PACKET_FLAG_RELIABLE);
}

void server::sendState(int netid2) {
    int32_t netid = netid2;
    int type, charstate, plantingtree, punchx, punchy;
    float x, y, xspeed, yspeed;
    type = PACKET_SET_CHARACTER_STATE;
    charstate = 0;
    int state = 0;
    state |= gt::ghost << 0; //ghost
    state |= doublejump << 1; //double jump
    state |= false << 1; //spirit form
    state |= false << 1; //hoveration
    state |= false << 1; //aurora
    state |= onering << 2; //one ring
    state |= false << 4; //mark of growganoth
    state |= noface << 4; //no face
    state |= false << 7; //halo
    state |= false << 13; //duct tape
    state |= false << 15; //lucky
    state |= false << 19; //geiger effect
    state |= false << 20; //spotlight
    state |= ssup << 24; //super suporter

    plantingtree = state;
    punchx = 0;
    punchy = 0;
    x = 1000.0f;
    y = 400.0f;
    xspeed = speed_x;

    yspeed = speed_y; //gravity

    BYTE* data = new BYTE[56];
    memset(data, 0, 56);
    memcpy(data + 0, &type, 4);
    memcpy(data + 4, &netid, 4);
    memcpy(data + 12, &charstate, 4);
    memcpy(data + 20, &plantingtree, 4);
    memcpy(data + 24, &x, 4);
    memcpy(data + 28, &y, 4);
    memcpy(data + 32, &xspeed, 4);
    memcpy(data + 36, &yspeed, 4);
    memcpy(data + 44, &punchx, 4);
    memcpy(data + 48, &punchy, 4);

    memcpy(data + 1, &punch_effect, 3);
    uint8_t build_range = 128;
    uint8_t punch_range = 128;
    memcpy(data + 2, &build_range, 1);
    memcpy(data + 3, &punch_range, 1);
    float waterspeed = 200.0f;
    memcpy(data + 16, &waterspeed, 4);
    cout << "\nPUNCH RANGE SET TO DEFAULT " + to_string(punch_range) + "\n";
    g_server->SendPacketRaw(true, 4, data, 56, 0, ENET_PACKET_FLAG_RELIABLE);



    /*
        768 = ghost
        256 = ghost gone
    */
}

void server::sendStateTest(int netid2, int statenum) {
    int32_t netid = netid2;
    int type, charstate, plantingtree, punchx, punchy;
    float x, y, xspeed, yspeed;
    type = PACKET_SET_CHARACTER_STATE;
    charstate = 0;
    int state = 0;
    state |= statenum;

    plantingtree = state;
    punchx = 0;
    punchy = 0;
    x = 1000.0f;
    y = 400.0f;
    xspeed = speed_x;

    yspeed = speed_y; //gravity

    BYTE* data = new BYTE[56];
    memset(data, 0, 56);
    memcpy(data + 0, &type, 4);
    memcpy(data + 4, &netid, 4);
    memcpy(data + 12, &charstate, 4);
    memcpy(data + 20, &plantingtree, 4);
    memcpy(data + 24, &x, 4);
    memcpy(data + 28, &y, 4);
    memcpy(data + 32, &xspeed, 4);
    memcpy(data + 36, &yspeed, 4);
    memcpy(data + 44, &punchx, 4);
    memcpy(data + 48, &punchy, 4);

    memcpy(data + 1, &punch_effect, 3);
    uint8_t build_range = 128;
    uint8_t punch_range = 128;
    memcpy(data + 2, &build_range, 1);
    memcpy(data + 3, &punch_range, 1);
    float waterspeed = 200.0f;
    memcpy(data + 16, &waterspeed, 4);
    g_server->SendPacketRaw(true, 4, data, 56, 0, ENET_PACKET_FLAG_RELIABLE);
}

void server::longpunch(int netid2, bool enable) {
    int32_t netid = netid2;
    int type, charstate, plantingtree, punchx, punchy;
    float x, y, xspeed, yspeed;
    type = PACKET_SET_CHARACTER_STATE;
    charstate = 0;
    punchx = 0;
    punchy = 0;
    x = 1000.0f;
    y = 400.0f;
    xspeed = speed_x;

    yspeed = speed_y; //gravity

    BYTE* data = new BYTE[56];
    memset(data, 0, 56);
    memcpy(data + 0, &type, 4);
    memcpy(data + 4, &netid, 4);
    memcpy(data + 12, &charstate, 4);
    memcpy(data + 24, &x, 4);
    memcpy(data + 28, &y, 4);
    memcpy(data + 32, &xspeed, 4);
    memcpy(data + 36, &yspeed, 4);

    memcpy(data + 1, &punch_effect, 3);
    uint8_t build_range = (enable ? -1 : 128);
    uint8_t punch_range = (enable ? -1 : 128);
    memcpy(data + 2, &build_range, 1);
    memcpy(data + 3, &punch_range, 1);

    g_server->SendPacketRaw(true, 4, data, 56, 0, ENET_PACKET_FLAG_RELIABLE);
}

void server::speedy(int netid, int XSpeed) {
    PlayerMovings data;
    data.packetType = 0x14;
    data.characterState = 0; // animation
    data.x = 1000;
    data.y = 100;
    data.punchX = 500;
    data.punchY = 0;
    data.XSpeed = 300;
    data.YSpeed = XSpeed;
    data.netID = netid;
    data.plantingTree = getState();
    BYTE* raw = packPlayerMovings(&data);
    int var = 0x808000;
    float water = 125.0f;
    memcpy(raw + 1, &var, 3);
    memcpy(raw + 16, &water, 4);
    g_server->SendPacketRaw(true, 4, raw, 56, 0, ENET_PACKET_FLAG_RELIABLE);
}

void send_state(int netid)
{

    int type, charstate, plantingtree, punchx, punchy;
    float x, y, xspeed, yspeed;
    type = PACKET_SET_CHARACTER_STATE;
    charstate = 0;
    int state = 0;
    state |= gt::ghost << 0; //ghost
    state |= true << 1; //double jump
    state |= false << 1; //spirit form
    state |= false << 1; //hoveration
    state |= false << 1; //aurora
    state |= false << 2; //one ring
    state |= false << 4; //mark of growganoth
    state |= false << 7; //halo
    state |= false << 13; //duct tape
    state |= false << 15; //lucky
    state |= false << 19; //geiger effect
    state |= false << 20; //spotlight
    state |= false << 24; //super suporter
    plantingtree = state;
    punchx = 0;
    punchy = 0;
    x = 1000.0f;
    y = 400.0f;
    xspeed = 250.0f;

    yspeed = 1000.0f; //gravity

    BYTE* data = new BYTE[56];
    memset(data, 0, 56);
    memcpy(data + 0, &type, 4);
    memcpy(data + 4, &netid, 4);
    memcpy(data + 12, &charstate, 4);
    memcpy(data + 20, &plantingtree, 4);
    memcpy(data + 24, &x, 4);
    memcpy(data + 28, &y, 4);
    memcpy(data + 32, &xspeed, 4);
    memcpy(data + 36, &yspeed, 4);
    memcpy(data + 44, &punchx, 4);
    memcpy(data + 48, &punchy, 4);

    memcpy(data + 1, &punch_effect, 3);
    uint8_t build_range = (false ? -1 : 128);
    uint8_t punch_range = (false ? -1 : 128);
    memcpy(data + 2, &build_range, 1);
    memcpy(data + 3, &punch_range, 1);
    float waterspeed = 200.0f;
    memcpy(data + 16, &waterspeed, 4);
    g_server->SendPacketRaw(true, 4, data, 56, 0, ENET_PACKET_FLAG_RELIABLE); //true before
    delete[] data;
}

void server::stateReset(int netid2) {
    int32_t netid = netid2;
    int type, charstate, plantingtree, punchx, punchy;
    float x, y, xspeed, yspeed;
    type = PACKET_SET_CHARACTER_STATE;
    charstate = 0;
    punchx = 0;
    punchy = 0;
    x = 1000.0f;
    y = 400.0f;
    xspeed = speed_x;

    yspeed = speed_y; //gravity

    BYTE* data = new BYTE[56];
    memset(data, 0, 56);
    memcpy(data + 0, &type, 4);
    memcpy(data + 4, &netid, 4);
    memcpy(data + 12, &charstate, 4);
    memcpy(data + 24, &x, 4);
    memcpy(data + 28, &y, 4);
    memcpy(data + 32, &xspeed, 4);
    memcpy(data + 36, &yspeed, 4);

    memcpy(data + 1, &punch_effect, 3);
    uint8_t build_range = 128;
    uint8_t punch_range = 128;
    memcpy(data + 2, &build_range, 1);
    memcpy(data + 3, &punch_range, 1);

    g_server->SendPacketRaw(true, 4, data, 56, 0, ENET_PACKET_FLAG_RELIABLE);
}

void server::addBlock(int tile, int x, int y) {
    PlayerMovings data;
    data.packetType = 0x3;
    data.characterState = 0x0; // animation
    data.x = x;
    data.y = y;
    data.punchX = x;
    data.punchY = y;
    data.XSpeed = 0;
    data.YSpeed = 0;
    data.netID = g_server->m_world.GetLocalPlayer().netid;
    data.plantingTree = tile;
    g_server->SendPacketRaw(true, 4, packPlayerMovings(&data), 56, 0, ENET_PACKET_FLAG_RELIABLE); // If client = false then auto ban
}

void server::placeBlock(bool client, int tile, int x, int y) {
    PlayerMovings data;
    data.packetType = PACKET_TILE_CHANGE_REQUEST;
    data.characterState = 0x0; // animation
    data.x = g_server->m_world.GetLocalPlayer().pos.m_x;
    data.y = g_server->m_world.GetLocalPlayer().pos.m_y;
    data.punchX = x;
    data.punchY = y;
    data.XSpeed = 0;
    data.XSpeed = 0;
    data.YSpeed = 0; gameupdatepacket_t* packet;
    data.netID = g_server->m_world.GetLocalPlayer().netid;
    data.plantingTree = tile;
    g_server->SendPacketRaw(client, 4, packPlayerMovings(&data), 56, 0, ENET_PACKET_FLAG_RELIABLE);
}

void server::wearItem(int itemid) {
    variantlist_t varlist{ "OnEquipNewItem" };
    varlist[1] = itemid;
    g_server->send(false, varlist, g_server->m_world.GetLocalPlayer().netid, -1);
    PlayerMovings data;
    g_server->SendPacketRaw(false, 4, packPlayerMovings(&data), 56, 0, ENET_PACKET_FLAG_RELIABLE);
}

void server::breakBlock(bool client, int x, int y) {
    PlayerMovings data;
    data.packetType = PACKET_TILE_CHANGE_REQUEST;
    data.characterState = 0x0; // animation
    data.x = g_server->m_world.GetLocalPlayer().pos.m_x;
    data.y = g_server->m_world.GetLocalPlayer().pos.m_y;
    data.punchX = x;
    data.punchY = y;
    data.XSpeed = 0;
    data.YSpeed = 0; gameupdatepacket_t* packet;
    data.netID = g_server->m_world.GetLocalPlayer().netid;
    data.plantingTree = 18;
    g_server->SendPacketRaw(client, 4, packPlayerMovings(&data), 56, 0, ENET_PACKET_FLAG_RELIABLE);
}

BYTE* GetStructPointerFromTankPacket(ENetPacket* packet)
{
    const unsigned int packetLenght = packet->dataLength;
    BYTE* result = nullptr;
    if (packetLenght >= 0x3C)
    {
        BYTE* packetData = packet->data;
        result = packetData + 4;
        if (*static_cast<BYTE*>(packetData + 16) & 8)
        {
            if (packetLenght < *reinterpret_cast<int*>(packetData + 56) + 60)
            {
                result = nullptr;
            }
        }
        else
        {
            int zero = 0;
            memcpy(packetData + 56, &zero, 4);
        }
    }
    return result;
}


void server::enterDoor(bool client, int x, int y) {
    PlayerMovings data2{};
    data2.packetType = 8;
    data2.plantingTree = 6;
    data2.characterState = 0; // animation
    data2.x = g_server->m_world.GetLocalPlayer().pos.m_x;
    data2.y = g_server->m_world.GetLocalPlayer().pos.m_y;
    data2.punchX = x;
    data2.punchY = y;
    data2.XSpeed = 0;
    data2.YSpeed = 0;
    data2.netID = g_server->m_world.GetLocalPlayer().netid;
    g_server->SendPacketRaw(client, 4, packPlayerMovings(&data2), 56, 0, ENET_PACKET_FLAG_RELIABLE);
}

void agabe(ENetPacket* packet) {
    auto lalal = GetStructPointerFromTankPacket(packet);
    PlayerMovings* data = unpackRaw(GetStructPointerFromTankPacket(packet));
    cout << "\n**************************************************\n";
    cout << "plantingTree: " << int(data->plantingTree) << std::endl;
    cout << "XSpeed: " << int(data->XSpeed) << std::endl;
    cout << "YSpeed: " << int(data->YSpeed) << std::endl;
    cout << "punchX: " << int(data->punchX) << std::endl;
    cout << "punchY: " << int(data->punchY) << std::endl;
    cout << "characterState: " << int(data->characterState) << std::endl;
    cout << "netID: " << int(data->netID) << std::endl;
    cout << "SecondaryNetID: " << int(data->SecondaryNetID) << std::endl;
    cout << "x: " << int(data->x) << std::endl;
    cout << "y: " << int(data->y) << std::endl;
    cout << "packetType: " << int(data->packetType) << std::endl;
    cout << "**************************************************\n";
}

void sendGemEffect(int x, int y, int count) {
    if (count != 0) {
        variantlist_t varlist{ "OnParticleEffect" };
        varlist[1] = 181;
        varlist[2] = vector2_t{ float(x), float(y) };
        varlist[3] = float(count);
        varlist[4] = float(0.000000);
        g_server->send(true, varlist);
    }
}
void showTotalGemsInTiles(int x, int y, int count) {
    sendGemEffect(x, y, count);
    return;
}

WorldTile* server::GetTile(int x, int y)
{
    if (x >= 0 && x < g_server->m_world.GetTileMap().worldSize.X && y >= 0 && y < g_server->m_world.GetTileMap().worldSize.Y)
    {
        int index = x + g_server->m_world.GetTileMap().worldSize.X * y;
        if (index >= 0 && index < static_cast<int>(g_server->m_world.GetTileMap().tiles.size()))
        {
            return &g_server->m_world.GetTileMap().tiles[index];
        }
    }
    return nullptr;
}

void server::handle_outgoing() {
    ENetEvent evt;
    while (enet_host_service(m_proxy_server, &evt, 0) > 0) { // ztz change 0 to 1
        m_gt_peer = evt.peer;

        switch (evt.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            if (!this->connect())
                return;
        } break;
        case ENET_EVENT_TYPE_RECEIVE: {
            int packet_type = get_packet_type(evt.packet);

            switch (packet_type) {
            case NET_MESSAGE_GENERIC_TEXT: {
                if (events::out::generictext(utils::get_text(evt.packet))) {
                    enet_packet_destroy(evt.packet);
                    return;
                }
            }break;
            case NET_MESSAGE_GAME_MESSAGE:
                if (events::out::gamemessage(utils::get_text(evt.packet))) {
                    enet_packet_destroy(evt.packet);
                    return;
                }
                break;
            case NET_MESSAGE_GAME_PACKET: {
                auto packet = utils::get_struct(evt.packet);
                auto packetGUP = utils::get_structGUP(evt.packet);

                if (!packet)
                    break;

                if (!packetGUP)
                    break;

                switch (packet->m_type) {

                case PACKET_STATE: {
                    if (events::out::state(packetGUP, packet)) {
                        enet_packet_destroy(evt.packet);
                        return;
                    }
                }break;

                case PACKET_TILE_CHANGE_REQUEST: {
                    if (pf) {
                        if (GetAsyncKeyState(VK_SHIFT)) {
                            enet_packet_destroy(evt.packet);
                            return;
                        }
                    }
                } break;

                case PACKET_ITEM_ACTIVATE_REQUEST: {
                    bool varmi = true;
                    if (packet->m_int_data != 1796 && packet->m_int_data != 242 && packet->m_int_data != 7188) varmi = false;
                    if (iswear && varmi == false) {

                        auto& world = g_server->m_world;
                        types types;
                        auto type = items[packet->m_int_data];
                        if (type.name.find("Ancestral") != -1 || type.name.find("Samille") != -1 || type.name.find("Chakram") != -1) {
                            ances = type.itemID == ances ? 0000.0 : type.itemID;
                        }
                        else
                            switch (type.clothingType) {
                            case 0:
                                hair = type.itemID == hair ? 0000.0 : type.itemID;
                                break;
                            case 1:
                                shirt = type.itemID == shirt ? 0000.0 : type.itemID;
                                break;
                            case 2:
                                pants = type.itemID == pants ? 0000.0 : type.itemID;
                                break;
                            case 3:
                                shoe = type.itemID == shoe ? 0000.0 : type.itemID;
                                break;
                            case 4:
                                face = type.itemID == face ? 0000.0 : type.itemID;
                                break;
                            case 5:
                                hand = type.itemID == hand ? 0000.0 : type.itemID;
                                break;
                            case 6:
                                back = type.itemID == back ? 0000.0 : type.itemID;

                                break;
                            case 7:
                                hair = type.itemID == hair ? 0000.0 : type.itemID;
                                break;
                            case 8:
                                neck = type.itemID == neck ? 0000.0 : type.itemID;
                                break;
                            default:
                                hair = type.itemID == hair ? 0000.0 : type.itemID;
                                break;
                            }
                        //int skincolor = 1685231359;

                        if (events::out::get_punch_id(packet->m_int_data) != 0) {
                            punch_effect = events::out::get_punch_id(packet->m_int_data);
                        }
                        if (back != 0000.0) {
                            doublejump = true;
                        }

                        if (packet->m_int_data == 3774 && noface == false) noface = true;
                        else if (packet->m_int_data == 3774 && noface == true) noface = false;

                        if (packet->m_int_data == 1904 && onering == false) onering = true;
                        else if (packet->m_int_data == 1904 && onering == true) onering = false;

                        sendState(world.GetLocalPlayer().netid);
                        variantlist_t liste{ "OnSetClothing" };
                        liste[1] = vector3_t{ hair,  shirt,  pants };
                        liste[2] = vector3_t{ shoe,  face,  hand };
                        liste[3] = vector3_t{ back,  mask,  neck };
                        liste[4] = skincolor;
                        liste[5] = vector3_t{ ances , 1.f, 0.f };
                        g_server->send(true, liste, world.GetLocalPlayer().netid, -1);

                        gameupdatepacket_t t{ 0 };
                        return;
                    }


                    int allah = g_server->local_player.inventory.getItemCount(packet->m_int_data);
                    if (allah == 0) return;


                }
                case PACKET_CALL_FUNCTION: {
                    if (events::out::variantlist(packet)) {
                        enet_packet_destroy(evt.packet);
                        return;
                    }
                }break;

                case PACKET_PING_REPLY:{
                    if (events::out::pingreply(packet)) {
                        enet_packet_destroy(evt.packet);
                        return;
                    }
                }break;
                case PACKET_DISCONNECT:
                case PACKET_APP_INTEGRITY_FAIL:
                    if (gt::in_game)
                        return;
                    break;

                default: break;
                }
            } break;
            case NET_MESSAGE_TRACK: 
            case NET_MESSAGE_CLIENT_LOG_RESPONSE: return;

            default: break; //PRINTS("Got unknown packet of type %d.\n", packet_type);
            }

            if (!m_server_peer || !m_real_server)
                return;
            enet_peer_send(m_server_peer, 0, evt.packet);
            enet_host_flush(m_real_server);
        } break;
        case ENET_EVENT_TYPE_DISCONNECT: {
            if (gt::in_game)
                return;
            if (gt::connecting) {
                this->disconnectsr(false);
                gt::connecting = false;
                return;
            }

        } break;
        default:  break; //PRINTS("UNHANDLED\n");
        }
    }
}

void server::handle_incoming() {
    ENetEvent event;

    while (enet_host_service(m_real_server, &event, 0) > 0) { // ztz change 0 to 1
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: PRINTC("Connection Event\n"); break;
        case ENET_EVENT_TYPE_DISCONNECT: this->disconnectsr(true); return;
        case ENET_EVENT_TYPE_RECEIVE: {
            if (event.packet->data) {
                int packet_type = get_packet_type(event.packet);
                switch (packet_type) {
                case NET_MESSAGE_GENERIC_TEXT: {
                    if (events::in::generictext(utils::get_text(event.packet))) {
                        enet_packet_destroy(event.packet);
                        return;
                    }
                }break;
                case NET_MESSAGE_GAME_MESSAGE: {
                    if (events::in::gamemessage(utils::get_text(event.packet))) {
                        enet_packet_destroy(event.packet);
                        return;
                    }
                }break;

                case NET_MESSAGE_GAME_PACKET: {
                    auto packet = utils::get_struct(event.packet);
                    if (!packet)
                        break;


                    
                    if (debug) {
                        if (packet) {
                            if (packet->m_type != 5 || packet->m_type != 34) {
                                printf("\n[INCOMING RAW PACKET]\n");
                                printf("Type: %s\n", utils::get_type_string(packet->m_type).c_str());
                                printf("Count: %d\n", (int)packet->m_count);
                                printf("Data: %d\n", (int)packet->m_data);
                                printf("Data Size: %d\n", (int)packet->m_data_size);
                                printf("Item: %d\n", (int)packet->m_item);
                                printf("Packet Flag: %d\n", (int)packet->m_packet_flags);
                                printf("Particle Time: %d\n", (int)packet->m_particle_time);
                                printf("Player Flags: %d\n", (int)packet->m_player_flags);
                                printf("NetID: %d\n", (int)packet->m_netid);
                                printf("PosX: %0f\n", (float)packet->m_vec_x);
                                printf("PosY: %0f\n", (float)packet->m_vec_y);
                                printf("State 1: %0f\n", (float)packet->m_state1);
                                printf("State 2: %0f\n", (float)packet->m_state2);
                                printf("Struct Flags: %0f\n", (float)packet->m_struct_flags);
                                printf("XSpeed: %d\n", (int)packet->m_vec2_x);
                                printf("YSpeed: %d\n", (int)packet->m_vec2_y);
                                printf("MainValue: %d\n", (int)packet->m_int_data);
                            }

                        }
                    }
                    

                    switch (packet->m_type) {

                    case PACKET_STATE: {
                        if (events::in::state(packet)) {
                            enet_packet_destroy(event.packet);
                            return;
                        }
                    }break;

                    case PACKET_CALL_FUNCTION: {
                        if (events::in::variantlist(packet)) {
                            enet_packet_destroy(event.packet);
                            return;
                        }
                    }break;

                    case PACKET_TILE_CHANGE_REQUEST: {
                        auto tile = m_world.GetTileMap().GetTileAtIndex(packet->m_state1, packet->m_state2);

                        if (packet->m_int_data == 18) {
                            if (tile->foreground != 0) tile->foreground = 0;
                            else tile->background = 0;
                        }
                        else {
                            auto item = item_map.find(packet->m_int_data);
                            if (item->second.actionType == 18)
                                tile->background = packet->m_int_data;

                            else tile->foreground = packet->m_int_data;

                            if (packet->m_player_flags == g_server->m_world.GetLocalPlayer().netid) {
                                auto s_items_ptr = &g_server->local_player.inventory.items;
                                auto it = s_items_ptr->find(packet->m_int_data);
                                if (it != s_items_ptr->end()) {
                                    if (it->second.count > 1)
                                        it->second.count -= 1;
                                    else s_items_ptr->erase(packet->m_int_data);
                                }
                            }
                        }
                    }break;

                    case PACKET_SEND_MAP_DATA: {
                        g_server->m_world.Serialize(utils::GetExtendedData(packet), packet);
                    }break;

                    case PACKET_SEND_TILE_UPDATE_DATA: {
                        int index = packet->m_state1 + packet->m_state2 * m_world.GetTileMap().worldSize.X;

                        if (index >= 0 && index < m_world.GetTileMap().tiles.size()) {
                            auto& tile = m_world.GetTileMap().tiles[index];
                            tile.Serialize(utils::GetExtendedData(packet), g_server->m_world.GetVersion());
                        }
                    }break;

                    case PACKET_TILE_APPLY_DAMAGE: {
                        int index = packet->m_state1 + packet->m_state2 * m_world.GetTileMap().worldSize.X;

                        if (index >= 0 && index < m_world.GetTileMap().tiles.size()) {
                            auto& tile = m_world.GetTileMap().tiles[index];

                            if (tile.foreground != 0) {
                                auto item = item_map.find(tile.foreground);

                                if (item->second.actionType == 12 || item->second.actionType == 31) {

                                    if ((tile.flag & WorldTile::eTileFlags::ENABLED) == WorldTile::eTileFlags::ENABLED) {
                                        tile.flag = static_cast<WorldTile::eTileFlags>(static_cast<std::uint16_t>(tile.flag) - 64);
                                    }
                                    else {
                                        tile.flag = static_cast<WorldTile::eTileFlags>(static_cast<std::uint16_t>(tile.flag) + 64);
                                    }
                                }

                            }
                        }
                    }break;

                    case PACKET_SEND_INVENTORY_STATE: {
                        local_player.inventory.slotCount = 0;
                        local_player.inventory.itemCount = 0;
                        local_player.inventory.items.clear();
                        std::vector<Item> invBuf;
                        LPBYTE extended_ptr = utils::get_extended(packet);
                        memcpy(&local_player.inventory.slotCount, extended_ptr + 5, 4);
                        memcpy(&local_player.inventory.itemCount, extended_ptr + 9, 2);
                        invBuf.resize(local_player.inventory.itemCount);
                        memcpy(invBuf.data(), extended_ptr + 11, invBuf.capacity() * sizeof(Item));
                        for (Item& item : invBuf)
                            local_player.inventory.items.operator[](item.id) = item;

                    }break;

                    case PACKET_SEND_TILE_TREE_STATE: {
                        auto tile = m_world.GetTileMap().GetTileAtIndex(packet->m_state1, packet->m_state2);
                        if (packet->m_item == -1) {
                            tile->foreground = 0x00;
                        }
                    }break;

                    case PACKET_MODIFY_ITEM_INVENTORY: {
                        auto packetGUP = utils::get_structGUP(event.packet);
                        auto s_ptr = &local_player.inventory.items;
                        if (packetGUP->gained_item_count != 0) s_ptr->operator[](packet->m_int_data).count += (packetGUP->gained_item_count);
                        else if (s_ptr->operator[](packet->m_int_data).count > packet->m_jump_amount) s_ptr->operator[](packet->m_int_data).count -= (packet->m_jump_amount);
                        else  s_ptr->erase(packet->m_int_data);
                    }break;

                    case PACKET_ITEM_CHANGE_OBJECT: {
                        if (packet->m_player_flags == -1) {
                            WorldObject item;
                            item.id = packet->m_int_data;
                            item.pos.X = packet->m_vec_x;
                            item.pos.Y = packet->m_vec_y;
                            item.count = static_cast<uint8_t>(packet->m_struct_flags);
                            item.flags = packet->m_packet_flags;
                            item.offset = ++m_world.GetObjectMap().lastOffset;
                            g_server->m_world.GetObjectMap().objects.push_back(item);
                            if (item.id == 112) {
                                gemsDrop = true;
                                instantDropX = ((packet->m_vec_x + 10) / 32);
                                instantDropY = ((packet->m_vec_y + 10) / 32);
                            }
                        }

                        else if (packet->m_player_flags == -3) {
                            for (auto& obj : g_server->m_world.GetObjectMap().objects) {
                                if (obj.id == packet->m_int_data && obj.pos.X == packet->m_vec_x && obj.pos.Y == packet->m_vec_y) {
                                    obj.count = static_cast<uint8_t>(packet->m_struct_flags);
                                    break;
                                }
                            }
                        }
                        else if (packet->m_player_flags > 0) {
                            for (int i = 0; i < g_server->m_world.GetObjectMap().objects.size(); i++) {
                                if (g_server->m_world.GetObjectMap().objects[i].offset == packet->m_int_data) {
                                    if (packet->m_player_flags == g_server->m_world.GetLocalPlayer().netid) {
                                        if (g_server->m_world.GetObjectMap().objects[i].id == 112) {
                                            local_player.gems_balance += g_server->m_world.GetObjectMap().objects[i].count;
                                        }
                                        else if (g_server->m_world.GetObjectMap().objects[i].id != 112) {
                                            auto s_items_ptr = &local_player.inventory.items;
                                            int buffer = local_player.inventory.getObjectAmountToPickUpUnsafe(g_server->m_world.GetObjectMap().objects[i]);
                                            if (!local_player.inventory.doesItemExistUnsafe(g_server->m_world.GetObjectMap().objects[i].id)) { //haven't tested yet.
                                                Item item;
                                                item.id = g_server->m_world.GetObjectMap().objects[i].id;
                                                item.count = buffer;
                                                item.type = 0;
                                                s_items_ptr->operator[](item.id) = item;
                                            }

                                            else s_items_ptr->operator[](g_server->m_world.GetObjectMap().objects[i].id).count += buffer;

                                            if (s_items_ptr->operator[](242).count >= 100) {
                                                gameupdatepacket_t drop{ 0 };
                                                drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                                                drop.m_int_data = 242;
                                                g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));

                                            }
                                        }
                                    }
                                    auto it = m_world.GetObjectMap().objects.begin() + i;
                                    g_server->m_world.GetObjectMap().objects.erase(it);
                                    break;
                                }
                            }
                        }

                    }break;

                    case PACKET_SEND_LOCK: {
                        auto tile = m_world.GetTileMap().GetTileAtIndex(packet->m_state1, packet->m_state2);

                        tile->foreground = packet->m_int_data;
                    }break;

                    case PACKET_PING_REQUEST: {
                        gameupdatepacket_t pkt = {};
                        pkt.m_type = PACKET_PING_REPLY;
                        pkt.m_int_data = packet->m_int_data;
                        pkt.m_vec_x = 64.f;
                        pkt.m_vec_y = 64.f;
                        pkt.m_vec2_x = 1000.f;
                        pkt.m_vec2_y = 250.f;
                        g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&pkt, sizeof(gameupdatepacket_t));
                        gt::send_log("Ping reply sent! (PACKET_PING_REQUEST)");
                    } break;

                    default: break;
                    }
                } break;

                case NET_MESSAGE_TRACK: {
                    if (events::in::tracking(utils::get_text(event.packet))) {
                        enet_packet_destroy(event.packet);
                        return;
                    }
                }break;
                case NET_MESSAGE_CLIENT_LOG_REQUEST: return;

                default:  break; //PRINTS("Got unknown packet of type %d.\n", packet_type);
                }
            }
            if (!m_gt_peer || !m_proxy_server)
                return;
            enet_peer_send(m_gt_peer, 0, event.packet);
            enet_host_flush(m_proxy_server);
        } break;
        default: break;
        }
    }
}

void server::lockThread()
{
    if (threadID != std::hash<std::thread::id>{}(std::this_thread::get_id())) {
        if (mutexStatus.load()) {
            mutexStatus.store(false);
            this->cv.notify_all();
        }
    }
}
void server::unlockThread()
{
    if (threadID != std::hash<std::thread::id>{}(std::this_thread::get_id())) {
        if (!mutexStatus.load()) {
            mutexStatus.store(true);
            this->cv.notify_all();
        }
    }
}

void server::poll() {
    //outgoing packets going to real server that are intercepted by our proxy server
    this->handle_outgoing();

    if (!m_real_server)
        return;

    //ingoing packets coming to gt client intercepted by our proxy client
    this->handle_incoming();
}

bool server::start() {

    ENetAddress address;
    enet_address_set_host(&address, "0.0.0.0");
    //address.port = m_port;
    address.port = m_proxyport;
    m_proxy_server = enet_host_create(&address, 1024, 10, 0, 0);
    //m_proxy_server = enet_host_create(&address, 1, 0, 0, 0);
    m_proxy_server->usingNewPacket = false;
    this->threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
    if (!m_proxy_server) {
        print::set_info("[SERVER]: ", "Failed to Start the Proxy Server!\n", Red);
        return false;
    }
    m_proxy_server->checksum = enet_crc32;
    auto code = enet_host_compress_with_range_coder(m_proxy_server);
    if (code != 0)
        print::set_info("[ENET]: ", "Enet Host Compressing Failed\n", Red);
    print::set_info("[SERVER]: ", "Started Enet Server.\n", LightBlue);
    return setup_client();
}


void server::quit() {
    gt::in_game = false;
    this->disconnectsr(true);
}

bool server::setup_client() {
    m_real_server = enet_host_create(0, 1, 2, 0, 0);
    //m_real_server = enet_host_create(0, 1, 0, 0, 0);
    m_real_server->usingNewPacket = true;
    if (!m_real_server) {
        print::set_info("[CLIENT]: ", "Failed to Start the Client\n", Red);
        return false;
    }
    m_real_server->checksum = enet_crc32;
    auto code = enet_host_compress_with_range_coder(m_real_server);
    if (code != 0)
        print::set_info("[ENET]: ", "Enet Host Compressing Failed\n", Red);
    enet_host_flush(m_real_server);
    print::set_info("[CLIENT]: ", "Started Enet Client\n", LightBlue);
    return true;
}


void server::redirect_server(variantlist_t& varlist) {
    m_port = varlist[1].get_uint32();
    m_token = varlist[2].get_uint32();
    m_user = varlist[3].get_uint32();
    auto str = varlist[4].get_string();

    auto doorid = str.substr(str.find("|"));
    m_server = str.erase(str.find("|")); //remove | and doorid from end
    PRINTC("Port: %d Token: %d User: %d Server: %s Doorid: %s\n", m_port, m_token, m_user, m_server.c_str(), doorid.c_str());
    //varlist[1] = m_port;
    varlist[1] = m_proxyport;
    varlist[4] = "127.0.0.1" + doorid;

    gt::connecting = true;
    send(true, varlist);
    if (m_real_server) {
        enet_host_destroy(m_real_server);
        m_real_server = nullptr;
    }
}

void server::disconnectsr(bool reset) {
    m_world.connected = false;
    m_world.GetLocalPlayer() = {};
    m_world.GetPlayers().clear();
    if (m_server_peer) {
        enet_peer_disconnect(m_server_peer, 0);
        m_server_peer = nullptr;
        enet_host_destroy(m_real_server);
        m_real_server = nullptr;
        //cout << "Disconnected" << endl;
    }
    if (reset) {
        //m_user = 0;
        //m_token = 0;
        //m_server = m_server;
        //m_port = m_port;//17198;// 17022;
        //cout << "Disconnected reset" << endl;
    }
    //cout << "Disconnected not reset or server_peer" << endl;
    print::set_info("[PROXY]: ", " Disconnected...\n", Red);

    /*
using namespace httplib;
Headers Header;
Header.insert(std::make_pair("User-Agent", "UbiServices_SDK_2019.Release.27_PC64_unicode_static"));
//Header.insert(std::make_pair("User-Agent", "UbiServices_SDK_2022.Release.9_PC64_ansi_static"));
Header.insert(make_pair("Host", "www.growtopia1.com"));
//Client cli("https://a104-125-3-135.deploy.static.akamaitechnologies.com");
Client cli("https://104.125.3.135");
//Client cli("https://silviozas.com");
cli.set_default_headers(Header);
cli.enable_server_certificate_verification(false);
cli.set_connection_timeout(2, 0);
auto res = cli.Post("/growtopia/server_data.php");
if (res.error() == Error::Success)
{
    rtvar var = rtvar::parse({ res->body });
    g_server->m_server = (var.find("server") ? var.get("server") : g_server->m_server);
    g_server->m_port = (var.find("port") ? var.get_int("port") : g_server->m_port);
    g_server->meta = (var.find("meta") ? var.get("meta") : (g_server->meta = var.get("meta")));

    print::set_info("[SERVER]: ", std::string("Server Address: ").append(g_server->m_server).c_str(), LightBlue);
    print::set_info("\n[SERVER]: ", (std::string("Server Port: ") + std::to_string(g_server->m_port) + "\n").c_str(), LightBlue);
}
}
catch (const std::exception& e)
{
    //cout << "Request Failed, error:" << e.what() << endl;
    //std::cerr << "Request failed, error: " << e.what() << '\n';
}
    */
}

bool server::connect() {
    print::set_info("[PROXY]: ", " Connecting to Server.\n", LightBlue);
    ENetAddress address;
    enet_address_set_host(&address, m_server.c_str());
    address.port = m_port;
    PRINTS("Port: %d Server: %s\n", m_port, m_server.c_str());
    if (!this->setup_client()) {
        print::set_info("[PROXY]: ", "Failed to setup client when trying to connect to server!\n", Red);
        return false;
    }
    m_server_peer = enet_host_connect(m_real_server, &address, 2, 0);
    if (!m_server_peer) {
        print::set_info("[PROXY]: ", "Failed to connect to real server.\n", Red);
        return false;
    }
    return true;
}



void server::SendPacketRaw2(bool client, int a1, void* packetData, size_t packetDataSize, void* a4, int packetFlag)
{
    ENetPacket* p;
    auto peer = client ? m_gt_peer : m_server_peer;
    if (peer) {
        if (a1 == 4 && *((BYTE*)packetData + 12) & 8)
        {
            p = enet_packet_create(0, packetDataSize + *((DWORD*)packetData + 13) + 5, packetFlag);
            int four = 4;
            memcpy(p->data, &four, 4);
            memcpy((char*)p->data + 4, packetData, packetDataSize);
            memcpy((char*)p->data + packetDataSize + 4, a4, *((DWORD*)packetData + 13));
            enet_peer_send(peer, 0, p);
        }
        else
        {
            p = enet_packet_create(0, packetDataSize + 5, packetFlag);
            memcpy(p->data, &a1, 4);
            memcpy((char*)p->data + 4, packetData, packetDataSize);
            enet_peer_send(peer, 0, p);
        }
    }
}
void server::SendPacketRaw(bool client, int a1, void* packetData, size_t packetDataSize, void* a4, int packetFlag) {
    ENetPacket* p;
    auto peer = client ? m_gt_peer : m_server_peer;
    if (peer) {
        if (a1 == 4 && *((BYTE*)packetData + 12) & 8) {
            p = enet_packet_create(0, packetDataSize + *((DWORD*)packetData + 13) + 5, packetFlag);
            int four = 4;
            memcpy(p->data, &four, 4);
            memcpy((char*)p->data + 4, packetData, packetDataSize);
            memcpy((char*)p->data + packetDataSize + 4, a4, *((DWORD*)packetData + 13));
            enet_peer_send(peer, 0, p);
        }
        else {
            p = enet_packet_create(0, packetDataSize + 5, packetFlag);
            memcpy(p->data, &a1, 4);
            memcpy((char*)p->data + 4, packetData, packetDataSize);
            enet_peer_send(peer, 0, p);
        }
    }
    delete (char*)packetData;
}


bool server::sendEnetPacket(ENetPacket* packet, bool client)
{
    auto peer = client ? m_gt_peer : m_server_peer;
    auto host = client ? m_proxy_server : m_real_server;
    if (!peer || !host)
    {
        //printf("The packet could not be sent due to the peer or host its null. Type %s", (client == true) ? "Growtopia" : "Local");
        enet_packet_destroy(packet);
        goto failed;
    }
    if (peer->state != ENET_PEER_STATE_CONNECTED)
    {
        //printf("The packet could not be sent due to the peer state not connected.");
        enet_packet_destroy(packet);
        goto failed;
    }
    if (enet_list_size(&host->peers->sentReliableCommands) > 45)
    {
        //printf("Packets have been cleared due to an excessive accumulation of packets.");
        enet_list_clear(&host->peers->sentReliableCommands);
        enet_packet_destroy(packet);
        goto failed;
    }
    if (enet_peer_send(peer, 0, packet) != 0)
    {
        //printf("The packet could not be sent due to the enet_peer_send function return false");
        enet_packet_destroy(packet);
        goto failed;
    }
    else
        enet_host_flush(host);
failed:
    return false;
}

//bool client: true - sends to growtopia client    false - sends to gt server
void server::send(bool client, int32_t type, uint8_t* data, int32_t len) {
    auto peer = client ? m_gt_peer : m_server_peer;
    auto host = client ? m_proxy_server : m_real_server;

    if (!peer || !host)
        return;
    auto packet = enet_packet_create(0, len + 5, ENET_PACKET_FLAG_RELIABLE);
    auto game_packet = (gametextpacket_t*)packet->data;
    game_packet->m_type = type;
    if (data)
        memcpy(&game_packet->m_data, data, len);

    memset(&game_packet->m_data + len, 0, 1);
    int code = enet_peer_send(peer, 0, packet);
    if (code != 0)
        PRINTS("Error Sending Packet! Code: %d\n", code);
    enet_host_flush(host);
}



//bool client: true - sends to growtopia client    false - sends to gt server
void server::send(bool client, variantlist_t& list, int32_t netid, int32_t delay) {
    auto peer = client ? m_gt_peer : m_server_peer;
    auto host = client ? m_proxy_server : m_real_server;

    if (!peer || !host)
        return;

    uint32_t data_size = 0;
    void* data = list.serialize_to_mem(&data_size, nullptr);

    //optionally we wouldnt allocate this much but i dont want to bother looking into it
    auto update_packet = MALLOC(gameupdatepacket_t, +data_size);
    auto game_packet = MALLOC(gametextpacket_t, +sizeof(gameupdatepacket_t) + data_size);

    if (!game_packet || !update_packet)
        return;

    memset(update_packet, 0, sizeof(gameupdatepacket_t) + data_size);
    memset(game_packet, 0, sizeof(gametextpacket_t) + sizeof(gameupdatepacket_t) + data_size);
    game_packet->m_type = NET_MESSAGE_GAME_PACKET;

    update_packet->m_type = PACKET_CALL_FUNCTION;
    update_packet->m_player_flags = netid;
    update_packet->m_packet_flags |= 8;
    update_packet->m_int_data = delay;
    memcpy(&update_packet->m_data, data, data_size);
    update_packet->m_data_size = data_size;
    memcpy(&game_packet->m_data, update_packet, sizeof(gameupdatepacket_t) + data_size);
    free(update_packet);

    auto packet = enet_packet_create(game_packet, data_size + sizeof(gameupdatepacket_t), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(host);
    free(game_packet);
}


//bool client: true - sends to growtopia client    false - sends to gt server
void server::send(bool client, std::string text, int32_t type) {
    auto peer = client ? m_gt_peer : m_server_peer;
    auto host = client ? m_proxy_server : m_real_server;

    if (!peer || !host)
        return;
    auto packet = enet_packet_create(0, text.length() + 5, ENET_PACKET_FLAG_RELIABLE);
    auto game_packet = (gametextpacket_t*)packet->data;
    game_packet->m_type = type;
    memcpy(&game_packet->m_data, text.c_str(), text.length());

    memset(&game_packet->m_data + text.length(), 0, 1);
    int code = enet_peer_send(peer, 0, packet);
    if (code != 0)
        PRINTS("Error Sending Packet! Code: %d\n", code);
    enet_host_flush(host);
}

void server::MoveXY(int x, int y) {
    try {
        auto& world = g_server->m_world;

        PathFinder pf(100, 60);

        for (int xx = 0; xx < 100; xx++) {
            for (int yy = 0; yy < 60; yy++) {
                int inta = 0;
                //auto tile = world.tiles.find(HashCoord(xx, yy));
                auto tile = world.GetTileMap().GetTileAtIndex(xx, yy);

                int collisionType = item_map.find(tile->foreground)->second.collisionType;
                if (collisionType == 0) {
                    inta = 0;
                }
                else if (collisionType == 1) {
                    inta = 1;
                }
                else if (collisionType == 2) {
                    inta = (floor(m_world.GetLocalPlayer().pos.m_y / 32) <= yy ? 1 : 0); // < to <=
                }

                else if (collisionType == 3) {
                    if (hasAccessName(localPlayerName)) {
                        inta = 0;
                    }
                    else {
                        if (tile->flag == 34 || tile->flag == 128) {
                            inta = 0;
                        }
                        else {
                            inta = 1;
                        }
                    }
                }
                else if (collisionType == 4) {
                    inta = (tile->flag & WorldTile::eTileFlags::ENABLED) ? 0 : 1;
                }
                else if (collisionType == 7) {
                    inta = (floor(m_world.GetLocalPlayer().pos.m_y / 32) >= yy ? 1 : 0); // > to >=
                }
                else {
                    inta = tile->foreground == 0 ? 0 : 1;
                }

                if (inta == 1) {
                    pf.setBlocked(xx, yy);
                }
                /*
                flags door//
                0 World locked and door locked
                128 World Locked but door open to public
                34 locked

                Airvent//
                0 Locked
                34 Not locked

                flags fissure// ct = 4
                    80 == open
                    16 == close
                */
            }
        }

        pf.setNeighbors({ -1, 0, 1, 0 }, { 0, 1, 0, -1 });
        vector<pair<int, int>> path = pf.aStar(g_server->m_world.GetLocalPlayer().pos.m_x / 32, g_server->m_world.GetLocalPlayer().pos.m_y / 32, x, y);

        if (path.size() > 0) {
            if (path.size() < 150)
                for (auto& p : path) {
                    gameupdatepacket_t packet{ 0 };
                    packet.m_type = PACKET_STATE;
                    packet.m_int_data = 6326;//6326;//822;

                    packet.m_vec_x = p.first * 32;
                    packet.m_vec_y = p.second * 32;
                    packet.m_vec2_x = p.first * 32;
                    packet.m_vec2_y = p.second * 32;
                    packet.m_state1 = p.first;
                    packet.m_state2 = p.second;
                    packet.m_packet_flags = 32 | 128 | (1 << 10) | (1 << 11);//UPDATE_PACKET_PLAYER_MOVING_RIGHT;
                    g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(gameupdatepacket_t));

                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
                }
            else {
                for (std::size_t i = 0; i < path.size(); i += 2) {
                    gameupdatepacket_t packet{ 0 };
                    packet.m_type = PACKET_STATE;
                    packet.m_int_data = 6326;//6326;//822;
                    packet.m_vec_x = path[i].first * 32;
                    packet.m_vec_y = path[i].second * 32;
                    packet.m_vec2_x = path[i].first * 32;
                    packet.m_vec2_y = path[i].second * 32;
                    packet.m_state1 = path[i].first;
                    packet.m_state2 = path[i].second;
                    packet.m_packet_flags = 32 | 128 | (1 << 10) | (1 << 11); //UPDATE_PACKET_PLAYER_MOVING_RIGHT;
                    g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(gameupdatepacket_t));

                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2));

            variantlist_t lost{ "OnSetPos" };
            vector2_t pos;
            pos.m_x = x * 32;
            pos.m_y = y * 32;
            lost[1] = pos;
            g_server->send(true, lost, g_server->m_world.GetLocalPlayer().netid, -1);
            variantlist_t notif{ "OnTextOverlay" };
            notif[1] = "`2Travelled `9" + to_string(path.size()) + " `2Blocks";
            g_server->send(true, notif, -1, -1);
        }
        else {
            variantlist_t notif{ "OnTextOverlay" };
            notif[1] = "`4Path Not Found";
            g_server->send(true, notif, -1, -1);
        }
    }
    catch (exception ex)
    {
        hasPf = true;
        variantlist_t notif{ "OnTextOverlay" };
        notif[1] = "`8Something Goes Wrong";
        g_server->send(true, notif, -1, -1);
    }
}