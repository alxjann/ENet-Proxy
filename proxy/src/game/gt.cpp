#include "gt.hpp"
#include "../packet/packet.h"
#include "../server/server.h"
#include "../utils/utils.h"
#include "../events/events.h"
#include "../pathfinder/Pathfinder2.h"

std::string gt::version = "3.99";
std::string gt::flag = "mm";
bool gt::resolving_uid2 = false;
bool gt::connecting = false;
bool gt::in_game = false;
bool gt::ghost = false;
bool gt::resolving_uid = false;
bool gt::aapbypass = false;
bool gt::ruzgar = false;
bool gt::noclip = false;
bool gt::doublejump = false;
bool gt::autocollect = false;
int gt::ac_range = 10;
int gt::path_amount = 0;

bool gt::noclip2 = false;
int gt::block_id = 0;
int gt::max_dropped_block = 0;
extern int max_dropped_block;
bool gt::showxy = false;
std::string gt::macaddr = "16:69:f9:t6:ga:6b";

void gt::send_log(std::string text) {
    g_server->send(true, "action|log\nmsg|" + text, NET_MESSAGE_GAME_MESSAGE);
}

bool hasAccessName(string nameAccess) {
    if (nameAccess.find("``") != -1 && (nameAccess.find("`^") != -1 || nameAccess.find("`2") != -1)) {
        return true;
    }
    else {
        return false;
    }
}

void gt::findpath(int x, int y)
{
    try {
        auto& world = g_server->m_world;

        PathFinder pf(100, 60);

        for (int xx = 0; xx < 100; xx++) {
            for (int yy = 0; yy < 60; yy++) {
                int inta = 0;
                auto tile = world.GetTileMap().GetTileAtIndex(xx, yy);

                int collisionType = item_map.find(tile->foreground)->second.collisionType;
                if (collisionType == 0) {
                    inta = 0;
                }
                else if (collisionType == 1) {
                    inta = 1;
                }
                else if (collisionType == 2) {
                    inta = (yy < yy ? 1 : 0);
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
                    inta = tile->flag == 64 ? 0 : 1;
                }
                else {
                    inta = tile->foreground == 0 ? 0 : 1;
                }

                if (inta == 1) {
                    pf.setBlocked(xx, yy);
                }
            }
        }

        pf.setNeighbors({ -1, 0, 1, 0 }, { 0, 1, 0, -1 });
        vector<pair<int, int>> path = pf.aStar(g_server->m_world.GetLocalPlayer().pos.m_x / 32, g_server->m_world.GetLocalPlayer().pos.m_y / 32, x, y);

        if (path.size() > 0) {
            if (path.size() < 150)
                for (auto& p : path) {
                    GameUpdatePacket packet{ 0 };
                    packet.type = PACKET_STATE;
                    packet.int_data = 2250;
                    packet.int_x = p.first;
                    packet.int_y = p.second;
                    packet.pos_x = p.first * 32;
                    packet.pos_y = p.second * 32;
                    packet.flags = 32 | (1 << 10) | (1 << 11);
                    g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(GameUpdatePacket));

                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
                }
            else {
                for (std::size_t i = 0; i < path.size(); i += 2) {
                    GameUpdatePacket packet{ 0 };
                    packet.type = PACKET_STATE;
                    packet.int_data = 2250;
                    packet.int_x = path[i].first;
                    packet.int_y = path[i].second;
                    packet.pos_x = path[i].first * 32;
                    packet.pos_y = path[i].second * 32;
                    packet.flags = 32 | (1 << 10) | (1 << 11);
                    g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(GameUpdatePacket));

                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2));

        }
    }
    catch (exception ex)
    {
        variantlist_t notif{ "OnTextOverlay" };
        notif[1] = "`8Something Goes Wrong";
        g_server->send(true, notif, -1, -1);
    }
}