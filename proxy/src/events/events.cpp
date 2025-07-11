#include "events.h"
#include "../utils/json.hpp"
#include "../game/gt.hpp"
#include "../../vendor/proton/hash.hpp"
#include "../../vendor/proton/rtparam.hpp"
#include "../../vendor/proton/variant.hpp"
#include "../server/server.h"
#include <vector> 
#include "../utils/utils.h"
#include <thread>
#include <limits.h>
#include "../itemManager/items_dat_decode.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../network/httplib.h"
#include "../player/player.h"
#include "../network/HTTPRequest.hpp"
#include "../packet/packet.h"
#include "../world/World.h"
#include "../../vendor/proton/klv.h"
#include <regex>
#include "../../vendor/proton/hash2.h"
#include <fstream>

bool reme = false, legit = false, qq = false;

std::vector<int> intToDigits(int num_)
{
    std::vector<int> ret;
    string iStr = to_string(num_);

    for (int i = iStr.size() - 1; i >= 0; --i)
    {
        int units = pow(10, i);
        int digit = num_ / units % 10;
        ret.push_back(digit);
    }

    return ret;
}



std::string removeColors(const std::string& nameWithColors) {
    std::regex colorRegex("`[0-9a-zA-Z@^]|`+$");
    return std::regex_replace(nameWithColors, colorRegex, "");
}


std::string readNamesFromFile() {
    std::string acclist = "";
    std::ifstream infile("name_password.txt");
    if (infile.is_open()) {
        std::string line;
        int growidNumber = 1; // Initialize growid number
        while (std::getline(infile, line)) {
            // Extract name from each line (before the first ':')
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::string name = line.substr(0, pos);
                acclist += "\nadd_button|growid" + std::to_string(growidNumber) + "|`9" + name + "``|noflags|0|0|";
                growidNumber++; // Increment growid number for next iteration
            }
        }
        infile.close();
    }
    else {
        std::cerr << "Failed to open file for reading.\n";
    }
    return acclist;
}


void writeAcc(const std::string& name, const std::string& password) {
    std::ofstream outfile("name_password.txt", std::ios_base::app); // Open the file in append mode
    if (outfile.is_open()) {
        outfile << name << ":" << password; // Write name and password to file
        outfile.close(); // Close the file
    }
    else {
        std::cerr << "Failed to open file for writing.\n";
    }
}


void send_vset() {
    if (iswear) {
        //int skincolor = 1685231359;
        variantlist_t liste{ "OnSetClothing" };
        liste[1] = vector3_t{ hair,  shirt,  pants };
        liste[2] = vector3_t{ shoe,  face,  hand };
        liste[3] = vector3_t{ back,  mask,  neck };
        liste[4] = skincolor;
        liste[5] = vector3_t{ ances , 1.f, 0.f };

        g_server->send(true, liste, g_server->m_world.GetLocalPlayer().netid, -1);
        g_server->sendState(g_server->m_world.GetLocalPlayer().netid);
    }
}
bool events::out::variantlist(gameupdatepacket_t* packet) {
    variantlist_t varlist{};
    varlist.serialize_from_mem(utils::get_extended(packet));
    if (debug) {
        PRINTS("\nVarlist: %s\n", varlist.print().c_str());
    }
    return false;
}

bool events::out::pingreply(gameupdatepacket_t* packet) {
    packet->m_vec2_x = 1000.f;  //gravity
    packet->m_vec2_y = 250.f;   //move speed
    packet->m_vec_x = 64.f;     //punch range
    packet->m_vec_y = 64.f;     //build range
    packet->m_jump_amount = 0;  //for example unlim jumps set it to high which causes ban
    packet->m_player_flags = 0; //effect flags. good to have as 0 if using mod noclip, or etc.
    gt::send_log("Ping reply sent! (pingreply)");
    return false;
}

bool isInside(float circle_x, float circle_y, float rad, float x, float y) {
    // Compare radius of circle with distance
    // of its center from given point
    if ((x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad)
        return true;
    else
        return false;
}

void SendPacket(int a1, string a2, ENetPeer* enetPeer)
{
    if (enetPeer)
    {
        ENetPacket* v3 = enet_packet_create(0, a2.length() + 5, 1);
        memcpy(v3->data, &a1, 4);
        //*(v3->data) = (DWORD)a1;
        memcpy((v3->data) + 4, a2.c_str(), a2.length());

        //cout << std::hex << (int)(char)v3->data[3] << endl;
        enet_peer_send(enetPeer, 0, v3);
    }
}

void save_details() {
    ifstream ifs("save/config.json");
    if (ifs.is_open()) {
        json j;
        ifs >> j;
        j["wm"] = wrench;
        j["ptp"] = pathfinding;
        j["modas"] = modas;
        j["modas3"] = ban_all;
        j["modas2"] = exit_world;
        ofstream f_("save/config.json");
        f_ << j;
        f_.close();
    }

}
int balance() {
    int balancewl = 0;
    balancewl = balancewl + g_server->local_player.inventory.getItemCount(242);
    balancewl = balancewl + g_server->local_player.inventory.getItemCount(1796) * 100;
    balancewl = balancewl + g_server->local_player.inventory.getItemCount(7188) * 10000;
    return balancewl;
}


int item_count(int itemid) {
    int wl = g_server->local_player.inventory.getItemCount(itemid);
    return wl;
}

bool daw()
{
    if (item_count(242) > 0) {
        dropwl = true;
        g_server->send(false, "action|drop\n|itemID|242");
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + to_string(item_count(242))); //242
    }
    if (item_count(1796) > 0) {
        dropdl = true;
        g_server->send(false, "action|drop\n|itemID|1796");
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + to_string(item_count(1796))); //242
    }
    if (item_count(7188) > 0) {
        dropwl = true;
        g_server->send(false, "action|drop\nitemID|7188");
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|7188|\ncount|" + to_string(item_count(7188))); //242
    }
    return true;
}

void autoc() {
    for (auto it = g_server->m_world.GetObjectMap().objects.begin(); it != g_server->m_world.GetObjectMap().objects.end(); ++it)
    {
        if (isInside(it->pos.X, it->pos.Y, 4 * 32, g_server->m_world.GetLocalPlayer().pos.m_x, g_server->m_world.GetLocalPlayer().pos.m_y))
        {
            GameUpdatePacket packet{ 0 };
            packet.pos_x = it->pos.X;
            packet.pos_y = it->pos.Y;
            packet.type = 11;
            packet.netid = -1;
            packet.object_id = it->offset;
            g_server->send(false, 4, (uint8_t*)&packet, sizeof(GameUpdatePacket));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void showAutoCollectEffectArea()
{
        while (showAutoCollectEffect)
        {


            if (g_server->m_world.connected)
            {
                float playerX = g_server->m_world.GetLocalPlayer().pos.m_x;
                float playerY = g_server->m_world.GetLocalPlayer().pos.m_y;

                int range = 3;
                for (int y = -range; y <= (range + 1); ++y) {
                //for (int y = -range; y <= range; ++y) {
                    for (int x = -range; x <= (range + 1); ++x) {
                        if (y == -range || y == (range + 1) || x == -range || x == (range + 1)) {
                            variantlist_t varlist{ "OnParticleEffect" };
                            varlist[1] = 30;
                            varlist[2] = vector2_t{ float(playerX + (x * 32)), float(playerY + (y * 32)) };
                            varlist[3] = 0;
                            varlist[4] = 0;
                            g_server->send(true, varlist);
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(400));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
}

void doAutoCollect() {
    while (autoCollect) {
        if (!g_server->m_world.connected) {
            cout << "PLAYER NOT IN WORLD TURN OFF AC\n";
            autoCollect = false;
            break;
        }
        for (auto& it : g_server->m_world.GetObjectMap().objects)
        {
            if (isInside(it.pos.X, it.pos.Y, 4 * 32, g_server->m_world.GetLocalPlayer().pos.m_x, g_server->m_world.GetLocalPlayer().pos.m_y))
           
            {
                GameUpdatePacket packet{ 0 };
                packet.pos_x = it.pos.X;
                packet.pos_y = it.pos.Y;
                packet.type = 11;
                packet.netid = -1;
                packet.object_id = it.offset;
                g_server->send(false, 4, (uint8_t*)&packet, sizeof(GameUpdatePacket));
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void autohost() {

    for (auto it = g_server->m_world.GetObjectMap().objects.begin(); it != g_server->m_world.GetObjectMap().objects.end(); ++it)
    {
        int itemposx = (it->pos.X + 10) / 32;
        int itemposy = (it->pos.Y + 10) / 32;
        if (isInside(it->pos.X, it->pos.Y, 8 * 32, g_server->m_world.GetLocalPlayer().pos.m_x, g_server->m_world.GetLocalPlayer().pos.m_y)) {
            if (it->id == 242 || it->id == 1796 || it->id == 7188) {
                //if (itemposx == ((int)autohost_pos1.m_x / 32) && itemposy == ((int)autohost_pos1.m_y / 32) || itemposx == ((int)autohost_pos2.m_x / 32) && itemposy == ((int)autohost_pos2.m_y / 32)) {
                if ((itemposx == ((int)autohost_pos1.m_x / 32) && itemposy == ((int)autohost_pos1.m_y / 32)) ||
                    (itemposx == ((int)autohost_pos2.m_x / 32) && itemposy == ((int)autohost_pos2.m_y / 32)) ||
                    ((autohost_pos3.m_x != 0 && itemposx == ((int)autohost_pos3.m_x / 32) && itemposy == ((int)autohost_pos3.m_y / 32))) ||
                    ((autohost_pos4.m_x != 0 && itemposx == ((int)autohost_pos4.m_x / 32) && itemposy == ((int)autohost_pos4.m_y / 32)))) {
                    GameUpdatePacket packet{ 0 };
                    packet.pos_x = it->pos.X;
                    packet.pos_y = it->pos.Y;
                    packet.type = 11;
                    packet.netid = -1;
                    packet.object_id = it->offset;
                    g_server->send(false, 4, (uint8_t*)&packet, sizeof(GameUpdatePacket));
                }
            }
        }
    }
}

void placeaga(int id, int posx, int posy) {
    if (posx == 0 || posy == 0)
    {
        gt::send_log("position cannot be null!");
        return;
    }
    auto pos = g_server->m_world.GetLocalPlayer().pos;
    GameUpdatePacket legitpacket{ 0 };
    legitpacket.type = PACKET_STATE;
    legitpacket.item_id = id;
    legitpacket.int_x = posx;
    legitpacket.int_y = posy;
    legitpacket.vec_x = pos.m_x;
    legitpacket.vec_y = pos.m_y;
    legitpacket.flags = 2592;
    // her �ey do�ruda aaqq

    g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&legitpacket, sizeof(GameUpdatePacket));

}


void TpRetard(float x, float y)
{
    vector2_t pos;
    pos.m_x = x;
    pos.m_y = y;
    variantlist_t varlist{ "OnSetPos" };
    varlist[1] = pos;
    g_server->m_world.GetLocalPlayer().pos = pos;
    g_server->send(true, varlist, g_server->m_world.GetLocalPlayer().netid, -1);
}

/*
int events::out::get_punch_id(const int id_) {
    switch (id_) {
    case 138: return 1;
    case 366:
    case 1464:
        return 2;
    case 472: return 3;
    case 594:
    case 10130:
    case 5424:
    case 5456:
    case 4136:
    case 10052:
        return 4;
    case 768: return 5;
    case 900:
    case 7760:
    case 9272:
    case 5002:
    case 7758:
        return 6;
    case 910:
    case 4332:
        return 7;
    case 930:
    case 1010:
    case 6382:
        return 8;
    case 1016:
    case 6058:
        return 9;
    case 1204:
    case 9534:
        return 10;
    case 1378: return 11;
    case 1440: return 12;
    case 1484:
    case 5160:
    case 9802:
        return 13;
    case 1512:
    case 1648:
        return 14;
    case 1542: return 15;
    case 1576: return 16;
    case 1676:
    case 7504:
        return 17;
    case 1748:
    case 8006:
    case 8008:
    case 8010:
    case 8012:
        return 19;
    case 1710:
    case 4644:
    case 1714:
    case 1712:
    case 6044:
    case 1570:
        return 18;
    case 1780: return 20;
    case 1782:
    case 5156:
    case 9776:
    case 9782:
    case 9810:
        return 21;
    case 1804:
    case 5194:
    case 9784:
        return 22;
    case 1868:
    case 1998:
        return 23;
    case 1874: return 24;
    case 1946:
    case 2800:
        return 25;
    case 1952:
    case 2854:
        return 26;
    case 1956: return 27;
    case 1960: return 28;
    case 2908:
    case 6312:
    case 9496:
    case 8554:
    case 3162:
    case 9536:
    case 4956:
    case 3466:
    case 4166:
    case 4506:
    case 2952:
    case 9520:
    case 9522:
    case 8440:
    case 3932:
    case 3934:
    case 8732:
    case 3108:
    case 9766:
    case 12368:
        return 29;
    case 1980: return 30;
    case 2066:
    case 4150:
    case 11082:
    case 11080:
    case 11078:
        return 31;
    case 2212:
    case 5174:
    case 5004:
    case 5006:
    case 5008:
        return 32;
    case 2218: return 33;
    case 2220: return 34;
    case 2266: return 35;
    case 2386: return 36;
    case 2388: return 37;
    case 2450:
        return 38;
    case 2476:
    case 4208:
    case 12308:
    case 10336:
    case 9804:
        return 39;
    case 4748:
    case 4294:
        return 40;
    case 2512:
    case 9732:
    case 6338:
        return 41;
    case 2572: return 42;
    case 2592:
    case 9396:
    case 2596:
    case 9548:
    case 9812:
        return 43;
    case 2720: return 44;
    case 2752: return 45;
    case 2754: return 46;
    case 2756: return 47;
    case 2802: return 49;
    case 2866: return 50;
    case 2876: return 51;
    case 2878:
    case 2880:
        return 52;
    case 2906:
    case 4170:
    case 4278:
        return 53;
    case 2886: return 54;
    case 2890: return 55;
    case 2910: return 56;
    case 3066: return 57;
    case 3124: return 58;
    case 3168: return 59;
    case 3214:
    case 9194:
        return 60;
    case 7408:
    case 3238:
        return 61;
    case 3274: return 62;
    case 3300: return 64;
    case 3418: return 65;
    case 3476: return 66;
    case 3596: return 67;
    case 3686: return 68;
    case 3716: return 69;
    case 4290: return 71;
    case 4474: return 72;
    case 4464:
    case 9500:
        return 73;
    case 4746: return 75;
    case 4778:
    case 6026: case 7784:
        return 76;
    case 4996:
    case 3680:
    case 5176:
        return 77;
    case 4840: return 78;
    case 5206: return 79;
    case 5480:
    case 9770:
    case 9772:
        return 80;
    case 6110: return 81;
    case 6308: return 82;
    case 6310: return 83;
    case 6298: return 84;
    case 6756: return 85;
    case 7044: return 86;
    case 6892: return 87;
    case 6966: return 88;
    case 7088:
    case 11020:
        return 89;
    case 7098:
    case 9032:
        return 90;
    case 7192: return 91;
    case 7136:
    case 9738:
        return 92;
    case 3166: return 93;
    case 7216: return 94;
    case 7196:
    case 9340:
        return 95;
    case 7392:
    case 9604:
        return 96;
    case 7384: return 98;
    case 7414: return 99;
    case 7402: return 100;
    case 7424: return 101;
    case 7470: return 102;
    case 7488: return 103;
    case 7586:
    case 7646:
    case 9778:
        return 104;
    case 7650: return 105;
    case 6804:
    case 6358:
        return 106;
    case 7568:
    case 7570:
    case 7572:
    case 7574:
        return 107;
    case 7668: return 108;
    case 7660:
    case 9060:
        return 109;
    case 7584:
        return 110;
    case 7736:
    case 9116:
    case 9118:
    case 7826:
    case 7828:
    case 11440:
    case 11442:
    case 11312:
    case 7830:
    case 7832:
    case 10670:
    case 9120:
    case 9122:
    case 10680:
    case 10626:
    case 10578:
    case 10334:
    case 11380:
    case 11326:
    case 7912:
    case 11298:
    case 10498:
    case 12342:
        return 111;
    case 7836:
    case 7838:
    case 7840:
    case 7842:
        return 112;
    case 7950: return 113;
    case 8002: return 114;
    case 8022: return 116;
    case 8036: return 118;
    case 9348:
    case 8372:
        return 119;
    case 8038: return 120;
    case 8816:
    case 8818:
    case 8820:
    case 8822:
        return 128;
    case 8910: return 129;
    case 8942: return 130;
    case 8944:
    case 5276:
        return 131;
    case 8432:
    case 8434:
    case 8436:
    case 8950:
        return 132;
    case 8946: case 9576: return 133;
    case 8960: return 134;
    case 9006: return 135;
    case 9058: return 136;
    case 9082:
    case 9304:
        return 137;
    case 9066:
        return 138;
    case 9136: return 139;
    case 9138:
        return 140;
    case 9172: return 141;
    case 9254: return 143;
    case 9256: return 144;
    case 9236: return 145;
    case 9342: return 146;
    case 9542: return 147;
    case 9378: return 148;
    case 9376: return 149;
    case 9410: return 150;
    case 9462: return 151;
    case 9606:
        return 152;
    case 9716:
    case 5192:
        return 153;
    case 10048: return 167;
    case 10064: return 168;
    case 10046: return 169;
    case 10050: return 170;
    case 10128: return 171;
    case 10210:
    case 9544:
        return 172;
    case 10330: return 178;
    case 10398: return 179;
    case 10388:
    case 9524:
    case 9598:
        return 180;
    case 10442: return 184;
    case 10506: return 185;
    case 10652: return 188;
    case 10676: return 191;
    case 10694: return 193;
    case 10714: return 194;
    case 10724: return 195;
    case 10722: return 196;
    case 10754: return 197;
    case 10800: return 198;
    case 10888: return 199;
    case 10886:
    case 11308:
        return 200;
    case 10890: return 202;
    case 10922: case 9550: return 203;
    case 10990: return 205;
    case 10998: return 206;
    case 10952: return 207;
    case 11000: return 208;
    case 11006: return 209;
    case 11046: return 210;
    case 11052: return 211;
    case 10960: return 212;
    case 10956:
    case 9774:
        return 213;
    case 10958: return 214;
    case 10954: return 215;
    case 11076: return 216;
    case 11084: return 217;
    case 11118:
    case 9546:
    case 9574:
        return 218;
    case 11120: return 219;
    case 11116: return 220;
    case 11158: return 221;
    case 11162: return 222;
    case 11142: return 223;
    case 11232: return 224;
    case 11140: return 225;
    case 11248:
    case 9596:
    case 9636:
        return 226;
    case 11240: return 227;
    case 11250: return 228;
    case 11284: return 229;
    case 11292: return 231;
    case 11314: return 233;
    case 11316: return 234;
    case 11324: return 235;
    case 11354: return 236;
    case 11760:
    case 11464:
    case 11438:
    case 12230:
    case 11716:
    case 11718:
    case 11674:
    case 11630:
    case 11786:
    case 11872:
    case 11762:
    case 11994:
    case 12172:
    case 12184:
    case 11460:
    case 12014:
    case 12016:
    case 12018:
    case 12020:
    case 12022:
    case 12024:
    case 12246:
    case 12248:
    case 12176:
    case 12242:
    case 11622:
    case 12350:
    case 12300:
    case 12374:
    case 12356:

    case 13486:
    case 13488:
    case 13490:
    case 13492:
    case 13494:
        return 237;
    case 11814:
    case 12232:
    case 12302:
        return 241;
    case 11548:
    case 11552:
        return 242;
    case 11704:
    case 11706:
        return 243;
    case 12180:
    case 12346:
    case 12344:
        return 244;
    case 11506:
    case 11508:
    case 11562:
    case 11768:
    case 11882:
    case 11720:
    case 11884:
        return 245;
    case 12432:
    case 12434:
        return 246;
    case 11818:
    case 11876:
    case 12000:
    case 12240:
    case 12642:
    case 12644:
        return 248;

    }
    return 0;
}
*/
int events::out::get_punch_id(const int id_) {
    switch (id_) {
    case 138:
    case 2976:
    case 8354:
        return 1;
    case 366:
    case 1464:
        return 2;
    case 472: return 3;
    case 594:
    case 10130:
    case 5424:
    case 5456:
    case 4136:
    case 10052:
        return 4;
    case 768: return 5;
    case 900:
    case 7760:
    case 9272:
    case 5002:
    case 7758:
    case 4664:
    case 8046:
    case 9062:
    case 1252:
    case 1254:
    case 9086:
    case 3680:
    case 5176:
    case 7750:
    case 1228:
    case 3430:
        return 6;
    case 910:
    case 4332:
    case 1250:
    case 12656:
        return 7;
    case 930:
    case 1010:
    case 6382:
    case 9698:
    case 6368:
        return 8;
    case 1016:
    case 6058:
        return 9;
    case 1204:
    case 9534:
    case 10928:
        return 10;
    case 1378: return 11;
    case 1440:
    case 4508:
    case 2224:
    case 4512:
    case 4510:
    case 4515:
    case 10996:
    case 9654:
    case 4514:
    case 11764:
    case 7748:
        return 12;
    case 1484:
    case 5160:
    case 9802:
        return 13;
    case 1512:
    case 1648:
        return 14;
    case 1542: return 15;
    case 1576: return 16;
    case 1676:
    case 7504:
        return 17;
    case 1748:
    case 8006:
    case 8008:
    case 8010:
    case 8012:
        return 19;
    case 1710:
    case 4644:
    case 1714:
    case 1712:
    case 6044:
    case 1570:
        return 18;
    case 3578: return 19;
    case 1780: return 20;
    case 1782:
    case 5156:
    case 9776:
    case 9810:
    case 10120:
        return 21;
    case 1804:
    case 5194:
    case 9784:
        return 22;
    case 1868:
    case 7754:
    case 1998:
        return 23;
    case 1874: return 24;
    case 1946:
    case 2800:
        return 25;
    case 1952:
    case 2854:
    case 9280:
    case 1974:
        return 26;
    case 1956: return 27;
    case 1960: return 28;
    case 2908:
    case 6312:
    case 9496:
    case 8554:
    case 3162:
    case 9536:
    case 8584:
    case 4956:
    case 3466:
    case 4166:
    case 2952:
    case 9520:
    case 9522:
    case 8440:
    case 3932:
    case 3934:
    case 7434:
    case 8732:
    case 3108:
    case 9766:
    case 12368:
    case 10780:
    case 3160:
    case 12102:
    case 4688:
    case 8604:
    case 3070:
    case 10402:
    case 7500:
    case 3206:
    case 3588:
    case 2636:
    case 8452:
    case 11066:
    case 9076:
    case 7890:
    case 8736:
    case 10936:
    case 10938:
        return 29;
    case 1980:
    case 7106:
    case 8042:
        return 30;
    case 2066:
    case 4150:
    case 11082:
    case 11080:
    case 9714:
    case 11078:
    case 3678:
    case 10686:
        return 31;
    case 2212:
    case 5174:
    case 5004:
    case 5006:
    case 5008:
    case 8912:
        return 32;
    case 2218: return 33;
    case 2220: return 34;
    case 2266: return 35;
    case 2386: return 36;
    case 2388: return 37;
    case 2450:
        return 38;
    case 2476:
    case 4208:
    case 12308:
    case 10336:
    case 9804:
        return 39;
    case 4294:
    case 1948:
        return 40;
    case 2512:
    case 9732:
    case 6338:
    case 6670:
    case 3736:
    case 10406:
    case 10232:
    case 10994:
    case 7146:
        return 41;
    case 2572:
    case 11072:
        return 42;
    case 2592:
    case 9396:
    case 2596:
    case 10930:
    case 9548:
    case 9812:
    case 9800:
    case 5158:
        return 43;
    case 2720: return 44;
    case 2752: return 45;
    case 2754:
    case 9830:
    case 9898:
        return 46;
    case 2756:
        return 47;
    case 2802: return 49;
    case 2866: return 50;
    case 2876: return 51;
    case 2878:
    case 2880:
        return 52;
    case 2906:
    case 4170:
    case 2888:
    case 4278:
    case 4126:
        return 53;
    case 2886: return 54;
    case 2890: return 55;
    case 2910: return 56;
    case 3066:
    case 10288:
        return 57;
    case 3124:
    case 5088:
        return 58;
    case 3168:
    case 3166:
        return 59;
    case 3214:
    case 9194:
    case 4506:
        return 60;
    case 7408:
    case 3238:
        return 61;
    case 3274: return 62;
    case 3300: return 64;
    case 3418: return 65;
    case 3476: return 66;
    case 3596: return 67;
    case 3686: return 68;
    case 3716:
    case 6086:
        return 69;
    case 4110:
    case 2986:
    case 4252:
        return 70;
    case 4290: return 71;
    case 4474: return 72;
    case 4464:
    case 9500:
        return 73;
    case 4660: return 74;
    case 4746:
    case 4750:
    case 4748:
        return 75;
    case 4778:
    case 6026: case 7784:
        return 76;
    case 4996:
        return 77;
    case 4840: return 78;
    case 5206: return 79;
    case 5480:
    case 9770:
    case 9772:
    case 9906:
    case 9908:
    case 9918:
    case 10290:
    case 10362:
        return 80;
    case 6110: return 81;
    case 6308: return 82;
    case 6310: return 83;
    case 6298: return 84;
    case 6756: return 85;
    case 7044: return 86;
    case 6892: return 87;
    case 6966: return 88;
    case 7088:
    case 11020:
        return 89;
    case 7098:
    case 9032:
    case 10384:
        return 90;
    case 7192: return 91;
    case 7136:
    case 11788:
        return 92;
    case 7142: return 93;
    case 7216: return 94;
    case 7196:
        return 95;
    case 7392:
    case 9604:
        return 96;
    case 6754: return 97;
    case 7384: return 98;
    case 7414: return 99;
    case 7402:
    case 7396:
        return 100;
    case 7424: return 101;
    case 7470:
    case 9738:
        return 102;
    case 7488: return 103;
    case 7586:
    case 9778:
        return 104;
    case 7650: return 105;
    case 6804:
    case 6358:
    case 7646:
        return 106;
    case 7568:
    case 7570:
    case 7572:
    case 7574:
        return 107;
    case 7668: return 108;
    case 7660:
    case 9060:
        return 109;
    case 7584:
        return 110;
    case 7736:
    case 9116:
    case 9118:
    case 7826:
    case 7828:
    case 11440:
    case 11442:
    case 11312:
    case 7830:
    case 7832:
    case 10670:
    case 9120:
    case 9122:
    case 10680:
    case 10626:
    case 10578:
    case 10334:
    case 11380:
    case 11326:
    case 7912:
    case 11298:
    case 10498:
    case 7940:
    case 12342:
    case 8492:
    case 9340:
    case 11358:
        return 111;
    case 7836:
    case 7838:
    case 7840:
    case 7842:
        return 112;
    case 7950: return 113;
    case 8002: return 114;
    case 8022: return 116;
    case 8036: return 118;
    case 9348:
    case 8372:
    case 8810:
        return 119;
    case 8038:
    case 11990:
    case 8360:
    case 8510:
    case 8374:
        return 120;
    case 8358: return 121;
    case 8364: return 122;
    case 8438: return 123;
    case 10066:
    case 8494:
    case 11310:
    case 9360:
        return 126;
    case 8814: return 127;
    case 8816:
    case 8818:
    case 8820:
    case 8822:
        return 128;
    case 8910: return 129;
    case 8942: return 130;
    case 8944:
    case 5276:
    case 10940:
        return 131;
    case 8432:
    case 8434:
    case 8436:
    case 8950:
        return 132;
    case 8946:
    case 9576:
    case 9636:
        return 133;
    case 8960: return 134;
    case 9006: return 135;
    case 9058: return 136;
    case 9082:
    case 9304:
        return 137;
    case 9066:
        return 138;
    case 9136: return 139;
    case 9138:
        return 140;
    case 9172:
    case 9176:
        return 141;
    case 9190: return 142;
    case 9254: return 143;
    case 9256: return 144;
    case 9236: return 145;
    case 9342: return 146;
    case 9542: return 147;
    case 9378: return 148;
    case 9376: return 149;
    case 9410: return 150;
    case 9462: return 151;
    case 9606:
    case 9758:
        return 152;
    case 9716:
    case 5192:
    case 9764:
    case 9916:
        return 153;
    case 10048:
    case 9912:
        return 167;
    case 10064:
    case 10604:
        return 168;
    case 10046: return 169;
    case 10050: return 170;
    case 10128: return 171;
    case 10210:
    case 9544:
        return 172;
    case 10250: return 173;
    case 10246: return 175;
    case 10278: return 176;
    case 10292:
    case 7406:
    case 9450:
        return 177;
    case 10330: return 178;
    case 10392:
        return 179;
    case 10388:
    case 9524:
    case 9598:
        return 180;
    case 11620: return 181;
    case 10426: return 183;
    case 10442: return 184;
    case 10506: return 185;
    case 10494: return 186;
    case 10618: return 187;
    case 10652: return 188;
    case 10676: return 191;
    case 10674: return 192;
    case 10694: return 193;
    case 10714: return 194;
    case 10724: return 195;
    case 10722: return 196;
    case 10754: return 197;
    case 10800: return 198;
    case 10888: return 199;
    case 10886:
        return 200;
    case 10894: return 201;
    case 10890: return 202;
    case 9880:
    case 9782:
    case 9947:
    case 10922:
    case 9550:
    case 9974:
        return 203;
    case 10914: return 204;
    case 10990: return 205;
    case 10998: return 206;
    case 10952: return 207;
    case 11000: return 208;
    case 11006: return 209;
    case 11046: return 210;
    case 11052: return 211;
    case 10960: return 212;
    case 10956:
    case 9774:
    case 9896:
    case 10944:
        return 213;
    case 10958: return 214;
    case 10954: return 215;
    case 11076: return 216;
    case 11084:
    case 10020:
        return 217;
    case 11118:
    case 9546:
    case 9574:
    case 9874:
    case 9914:
        return 218;
    case 11120: return 219;
    case 11116: return 220;
    case 11158: return 221;
    case 11162: return 222;
    case 11142: return 223;
    case 11232: return 224;
    case 11140: return 225;
    case 11248:
    case 9596:
        return 226;
    case 11240: return 227;
    case 11250: return 228;
    case 11284: return 229;
    case 11292: return 231;
    case 11308: return 232;
    case 11314: return 233;
    case 11316: return 234;
    case 11324: return 235;
    case 11354: return 236;
    case 11760:
    case 11464:
    case 11438:
    case 12846:
    case 12230:
    case 11716:
    case 11718:
    case 11674:
    case 11630:
    case 11786:
    case 11872:
    case 11762:
    case 11994:
    case 12172:
    case 12184:
    case 11460:
    case 12014:
    case 12016:
    case 12018:
    case 12020:
    case 12022:
    case 12024:
    case 12246:
    case 12248:
    case 12176:
    case 12242:
    case 11622:
    case 12350:
    case 12300:
    case 12374:
    case 12356:
    case 12286:
    case 12628:
    case 12420:
    case 12384:
    case 12410:
    case 12412:
    case 12404:
    case 12402:
    case 12416:
    case 12658:
    case 11542:
    case 12860:
    case 12870:
    case 12862:
    case 12850:
    case 12886:
    case 12990:
    case 12992:
    case 12880:
    case 13060:
    case 13136:
    case 13114:
    case 13118:
    case 13190:
    case 13326:
    case 13330:
    case 13332:
    case 13366:
    case 13188:
    case 13410:
    case 13486:
    case 13488:
    case 13490:
    case 13492:
    case 13494:
    case 13484:
    case 13578:
    case 13552:
    case 13554:
    case 13572:
    case 13606:
        return 237;
    case 11384: return 239;
    case 11458: return 240;
    case 11814:
    case 12232:
    case 12302:
    case 12872:
    case 12874:
    case 12958:
    case 13324:
    case 13424:
        return 241;
    case 11548:
    case 11552:
        return 242;
    case 11704:
    case 11706:
        return 243;
    case 12180:
    case 12346:
    case 12344:
    case 13058:
    case 13498:
    case 13322:
        return 244;
    case 11506:
    case 11508:
    case 11562:
    case 11768:
    case 11882:
    case 11720:
    case 11884:
    case 13116:
    case 11536:
        return 245;
    case 12432:
    case 12434:
    case 12842:
    case 12640:
    case 13268:
        return 246;
    case 11818:
    case 11876:
    case 12000:
    case 12240:
    case 12642:
    case 12644:
    case 13022:
    case 13024:
    case 13396:
    case 13398:
    case 12564:
        return 248;

    }
    return 0;
}

bool dropitem(int itemid, string count) {
    cdrop = true;
    g_server->send(false, "action|drop\nitemID|" + std::to_string(itemid));
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + count); //242
    return true;
}


bool unaccessing = false;
void unaccess() {
    unaccessing = true;
    g_server->send(false, "action|input\n|text|/unaccess");
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    g_server->send(false, "action|dialog_return\ndialog_name|unaccess\nbuttonClicked|Yes"); //242
    gt::send_log("`2Done Unaccess.");
}
void tptopos(float x, float y)
{
    vector2_t pos;
    pos.m_x = x;
    pos.m_y = y;
    variantlist_t varlist{ "OnSetPos" };
    varlist[1] = pos;
    g_server->m_world.GetLocalPlayer().pos = pos;
    g_server->send(true, varlist, g_server->m_world.GetLocalPlayer().netid, -1);
}

bool find_command(std::string chat, std::string name) {
    bool found = chat.find("/" + name) == 0;
    if (found)
        gt::send_log("`6" + chat);
    return found;
}
void ban_everyone() {
    gt::send_log("`9You have `4(Ban All)`9 When Mod joins option `2Enabled");
    gt::send_log("`4Banning `9Everyone in the world...");
    std::string username = "all";
    for (auto& player : g_server->m_world.GetPlayers()) {
        auto nik = player.name.substr(2); //remove color
        string banint = player.name.substr(2).substr(0, player.name.length() - 4);
        if (nik.find(username)) {
            g_server->send(false, "action|input\n|text|/ban " + banint);
        }
    }
}

void exit_pasaulis() {
    gt::send_log("`9You have `4(Exit World)`9 When Mod joins option `2Enabled");
    gt::send_log("`bLeaving The World Now");
    g_server->send(false, "action|quit_to_exit", 3);
}

bool check_number(string num) {
    for (int i = 0; i < num.length(); i++)
        if (isdigit(num[i]) == false)
            return false;
    return true;
}

void anti_gravity() {
    GameUpdatePacket packet{ 0 };
    packet.type = PACKET_TILE_CHANGE_REQUEST;
    packet.item_id = 4992;
    packet.int_x = 99;
    packet.int_y = 59;
    g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(GameUpdatePacket));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    auto pos = g_server->m_world.GetLocalPlayer().pos;

    g_server->enterDoor(true, 99, 59);


    GameUpdatePacket legitpacket{ 0 };
    legitpacket.type = PACKET_STATE;
    legitpacket.item_id = 18;
    legitpacket.int_x = 99;
    legitpacket.int_y = 59;
    legitpacket.vec_x = pos.m_x;
    legitpacket.vec_y = pos.m_y;
    legitpacket.flags = 2592;
    g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&legitpacket, sizeof(GameUpdatePacket));
}

bool itsmod(int netid, std::string growid = "")
{
    if (modas) {
        if (netid == g_server->m_world.GetLocalPlayer().netid)
            return false;
        else if (growid.find(g_server->m_world.GetLocalPlayer().name) != std::string::npos)
            return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        for (auto g : g_server->m_world.GetPlayers())
        {
            std::this_thread::sleep_for(std::chrono::microseconds(5));
            std::transform(g.name.begin(), g.name.end(), g.name.begin(), [](unsigned char c) { return std::tolower(c); });
            if (netid == g.netid)
                return false;
            else if (growid.find(g.name) != std::string::npos)
                return false;
        }
        gt::send_log("`2A `#@Moderator `2Joined the World");
        variantlist_t varlist{ "OnAddNotification" };
        varlist[1] = "interface/atomic_button.rttex";
        varlist[2] = "`2A `#@Moderator `2Joined the World";
        varlist[3] = "audio/hub_open.wav";
        varlist[4] = 0;
        g_server->send(true, varlist);
        if (ban_all && exit_world) {
            ban_everyone();
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            unaccess();
            exit_pasaulis();

        }

        else if (ban_all)
        {
            ban_everyone();
            unaccess();
        }
        else if (exit_world) {
            exit_pasaulis();
            unaccess();

        }
        return true;
    }
}
bool worldOption(std::string option)
{
    std::string username = "all";
    for (auto& player : g_server->m_world.GetPlayers()) {
        auto name_2 = player.name.substr(2); //remove color
        std::transform(name_2.begin(), name_2.end(), name_2.begin(), ::tolower);
        if (name_2.find(username)) {
            auto& bruh = g_server->m_world.GetLocalPlayer();
            if (option == "pull")
            {
                string plyr = player.name.substr(2).substr(0, player.name.length() - 4);
                if (plyr != bruh.name.substr(2).substr(0, player.name.length() - 4))
                {
                    g_server->send(false, "action|input\n|text|/pull " + plyr);
                }
            }
            if (option == "kick")
            {
                string plyr = player.name.substr(2).substr(0, player.name.length() - 4);
                if (plyr != bruh.name.substr(2).substr(0, player.name.length() - 4))
                {
                    g_server->send(false, "action|input\n|text|/kick " + plyr);

                }
            }
            if (option == "ban")
            {
                string plyr = player.name.substr(2).substr(0, player.name.length() - 4);
                if (plyr != bruh.name.substr(2).substr(0, player.name.length() - 4))
                {
                    g_server->send(false, "action|input\n|text|/ban " + plyr);

                }
            }
        }
    }
    return true;
}

bool custom_drop(int sayi, vector2_t pos, float m_x, float m_y) {

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    string cdropcount = to_string(sayi);
    gt::send_log("CDROP: " + to_string(balance()));
    if (balance() < sayi) {
        gt::send_log("`9Dont have `#balance`9. balance: " + to_string(balance()) + ".");
        return true;
    }
    if (sayi < 100) {

        if (item_count(242) < sayi) {
            gameupdatepacket_t drop{ 0 };
            drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
            drop.m_int_data = 1796;
            g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        dropwl = true;
        g_server->send(false, "action|drop\n|itemID|242");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + cdropcount); //242
        gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
    }

    else if (sayi > 10000) {


        int sayi1 = (sayi / 10000);

        int kalan = ((sayi / 100) - (sayi1 * 100));
        int kalan2 = sayi - ((kalan * 100) + (sayi1 * 10000));
        if (kalan > item_count(1796)) {
            gameupdatepacket_t drop{ 0 };
            drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
            drop.m_int_data = 7188;
            g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else if (item_count(242) < kalan2) {
            gameupdatepacket_t drop{ 0 };
            drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
            drop.m_int_data = 1796;
            g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        dropbgl = true;
        g_server->send(false, "action|drop\n|itemID|7188");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|7188|\ncount|" + std::to_string(sayi1)); //242

        dropdl = true;
        g_server->send(false, "action|drop\n|itemID|1796");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string(kalan)); //242

        dropwl = true;
        g_server->send(false, "action|drop\n|itemID|242");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + std::to_string(kalan2)); //242

        gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
    }
    else {
        int sayi1 = (sayi / 100);
        int kalan = (sayi % 100);

        if (item_count(242) < kalan) {
            gameupdatepacket_t drop{ 0 };
            drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
            drop.m_int_data = 1796;
            g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

        }
        else if (item_count(1796) < sayi1) {
            gameupdatepacket_t drop{ 0 };
            drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
            drop.m_int_data = 242;
            g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

        }
        dropdl = true;
        g_server->send(false, "action|drop\n|itemID|1796");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string(sayi1)); //242

        dropwl = true;
        g_server->send(false, "action|drop\n|itemID|242");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + std::to_string(kalan)); //242

        gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
    }
    total_bet = 0;
}

int scanwldrop(vector2_t autohostpos) {
    int wlcount = 0;
    for (const auto& obj : g_server->m_world.GetObjectMap().objects) {
        int posX = (obj.pos.X + 10) / 32;
        int posY = (obj.pos.Y + 10) / 32;

        if (posX == int(autohostpos.m_x / 32) && posY == int(autohostpos.m_y / 32)) {
            if (obj.id == 242) {
                wlcount = wlcount + obj.count;
            }
            if (obj.id == 1796) {
                wlcount = (wlcount + obj.count) * 100;
            }
            if (obj.id == 7188) {
                wlcount = (wlcount + obj.count) * 10000;
            }
        }
    }
    return wlcount;
}

void fast_shop(int aga) {
    for (int i = 0; i < aga; i++) {
        g_server->send(false, "action|buy\nitem|" + lastshop, 2);
    }
    shopmode = false;
    gt::send_log(" bought " + to_string(aga) + " " + lastshop);
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
void totalGemsInSpecificTiles(int targetPosX, int targetPosY) {
    int gemCount = 0;
    //std::this_thread::sleep_for(std::chrono::milliseconds(200));
    for (const auto& obj : g_server->m_world.GetObjectMap().objects) {
        int posX = (obj.pos.X + 10) / 32;
        int posY = (obj.pos.Y + 10) / 32;

        // Check if the current object is at the target position
        if (posX == targetPosX && posY == targetPosY) {
            if (obj.id == 112) {
                gemCount = gemCount + obj.count;
            }
        }
    }

    if (gemCount != 0) showTotalGemsInTiles(targetPosX * 32 + 16, targetPosY * 32 + 16, gemCount);
    return;
}
void showGemsCollected() {
    showGemsCollected_running = true;
    std::thread([&]() {
        while (collected_gems) {
            oldGemCount = g_server->local_player.gems_balance;
            std::this_thread::sleep_for(std::chrono::milliseconds(325));

            if (oldGemCount != g_server->local_player.gems_balance) {
                std::this_thread::sleep_for(std::chrono::milliseconds(325));
                variantlist_t varlist2{ "OnTalkBubble" };
                varlist2[1] = g_server->m_world.GetLocalPlayer().netid;
                varlist2[2] = "Collected `#" + to_string(g_server->local_player.gems_balance - oldGemCount) + " `9Gems";
                varlist2[3] = 0;
                varlist2[4] = 0;
                g_server->send(true, varlist2);
                gt::send_log("`4[peanut] ``Collected `#" + to_string(g_server->local_player.gems_balance - oldGemCount) + " `9Gems");
            }
        }
        showGemsCollected_running = false;
        }).detach();
}
void showGemsInstantDrop() {
    showGemsInstantDrop_running = true;
    std::thread([&]() {
        while (instant_gems) {
            std::this_thread::sleep_for(std::chrono::milliseconds(325));
            if (gemsDrop) {
                std::this_thread::sleep_for(std::chrono::milliseconds(325));
                totalGemsInSpecificTiles(instantDropX, instantDropY);
                gemsDrop = false;
            }
        }
        showGemsInstantDrop_running = false;
        }).detach();
}


void sendVisualWeather(int weather) {
    variantlist_t varlist{ "OnSetCurrentWeather" };
    varlist[1] = weather;
    g_server->send(true, varlist);
}

void sendWrenchModeDialog() {
    string wrench_mode_on_off = (wrenchModeOnOff) ? "`2Enabled" : "`4Disabled";
    string wrench_mode_on_off_2 = (wrenchModeOnOff) ? "`4Disable" : "`2Enable";
    string wrench_right_kick = (wrenchRightKick) ? "`2Enabled" : "`4Disabled";
    string wrench_right_kick_2 = (wrenchRightKick) ? "`4Disable" : "`2Enable";
    std::string paket;
    paket =
        "\nadd_label_with_icon|big|`9Wrench Mode|left|32|"
        "\nadd_spacer|small|"
        "\nadd_textbox|`5 Mode: `0" + wrenchMode + "|left|"
        "\nadd_button_with_icon|wrench_pull|  `5PULL  |" + wrenchPullFrame + "|32|"
        "\nadd_button_with_icon|wrench_kick|  `5KICK |" + wrenchKickFrame + "|32|"
        "\nadd_button_with_icon|wrench_ban|  `5BAN  |" + wrenchBanFrame + "|32|"
        "\nadd_button_with_icon||END_LIST|noflags|0|0|"
        "\nadd_spacer|small|"
        "\nadd_smalltext|Wrench Mode is " + wrench_mode_on_off + "|"
        "\nadd_button|wrench_mode|" + wrench_mode_on_off_2 + " `9Wrench Mode|"
        "\nadd_spacer|small|"
        "\nadd_smalltext|Right Click Kick is " + wrench_right_kick + "|"
        "\nadd_button|wrench_right_kick|" + wrench_right_kick_2 + " `9Right Click Kick|"
        "\nadd_quick_exit|"
        "\nend_dialog|wrench_mode_dialog|`0Cancel|`0Okay|";
    variantlist_t liste{ "OnDialogRequest" };
    liste[1] = paket;
    g_server->send(true, liste);
}
void sendAutoWrenchDialog() {
    std::string paket;
    paket =
        "\nadd_label_with_icon|big|`9Auto Wrench Mode|left|32|"
        "\nadd_spacer|small|"
        "\nadd_textbox|`5 Mode: `0" + wrenchMode + "|left|"
        "\nadd_button_with_icon|auto_pull|  `5PULL  |" + autoPullFrame + "|32|"
        "\nadd_button_with_icon|auto_ban|  `5BAN |" + autoBanFrame + "|32|"
        "\nadd_button_with_icon|auto_wrench_disable|  `5DISABLE |staticBlueFrame|32|"
        "\nadd_button_with_icon||END_LIST|noflags|0|0|"

        "\nadd_spacer|small|"

        "\nadd_textbox|`5 Auto Pull: `0" + autoPullName + "|left|"
        "\nadd_text_input|auto_pull_name|Auto Pull Name:||20|"
        //"\nadd_button|auto_pull_list|`9Auto Pull List``|noflags|0|0|"
        //"\nadd_button|auto_ban_list|`9Auto Ban List``|noflags|0|0|"

        //"\nadd_quick_exit|"
        "\nend_dialog|auto_wrench_dialog|`0Cancel|`0Okay|";
    variantlist_t liste{ "OnDialogRequest" };
    liste[1] = paket;
    g_server->send(true, liste);
}


int playerFaceFlag;
void auto_fish(int id, int posx, int posy) {
    auto pos = g_server->m_world.GetLocalPlayer().pos;

    //g_server->enterDoor(false, posx, posy);

    GameUpdatePacket legitpacket{ 0 };
    legitpacket.type = PACKET_STATE;
    legitpacket.item_id = id;
    legitpacket.int_x = posx;
    legitpacket.int_y = posy;
    legitpacket.vec_x = pos.m_x;
    legitpacket.vec_y = pos.m_y;
    legitpacket.flags = playerFaceFlag;

    cout << "\n" + to_string(legitpacket.flags) + "\n";
    g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&legitpacket, sizeof(GameUpdatePacket));
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    GameUpdatePacket packet{ 0 };
    packet.type = PACKET_TILE_CHANGE_REQUEST;
    packet.item_id = id;
    packet.int_x = posx;
    packet.int_y = posy;
    packet.vec_x = pos.m_x;
    packet.vec_y = pos.m_y;
    g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(GameUpdatePacket));
}

void hitBlock(int x, int y) {
    GameUpdatePacket hitpacket{ 0 };
    hitpacket.type = PACKET_TILE_CHANGE_REQUEST;
    hitpacket.item_id = 18;
    hitpacket.int_x = x;
    hitpacket.int_y = y;
    hitpacket.pos_x = g_server->m_world.GetLocalPlayer().pos.m_x;
    hitpacket.pos_y = g_server->m_world.GetLocalPlayer().pos.m_y;
    g_server->send(false, NET_MESSAGE_GAME_PACKET, reinterpret_cast<uint8_t*>(&hitpacket), sizeof(GameUpdatePacket));
}

void placeBlock(int itemID, int x, int y) {
    GameUpdatePacket placepacket{ 0 };
    placepacket.type = PACKET_TILE_CHANGE_REQUEST;
    placepacket.item_id = itemID;
    placepacket.int_x = x;
    placepacket.int_y = y;
    placepacket.pos_x = g_server->m_world.GetLocalPlayer().pos.m_x;
    placepacket.pos_y = g_server->m_world.GetLocalPlayer().pos.m_y;
    g_server->send(false, NET_MESSAGE_GAME_PACKET, reinterpret_cast<uint8_t*>(&placepacket), sizeof(GameUpdatePacket));
}

void doAutofarm(int itemID, int x, int y) {
    while (auto_farm) {
        if (g_server->GetTile(x, y)->foreground == 0 || g_server->GetTile(x, y)->background == 0) {
            placeBlock(itemID, x, y);
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
        while (g_server->GetTile(x, y)->foreground == itemID || g_server->GetTile(x, y)->background == itemID) {
            hitBlock(x, y);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}


float distance(float x1, float y1, float x2, float y2) {
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

bool events::out::generictext(std::string packet) {

    if (iswear) send_vset();
    if (debug) PRINTS("\nGeneric text: %s\n", packet.c_str());
    auto& world = g_server->m_world;
    rtvar var = rtvar::parse(packet);
    if (!var.valid())
        return false;


    if (packet.find("action|refresh_item_data") != -1) {
        //g_server->send(false, "action|enter_game", NET_MESSAGE_GENERIC_TEXT);
        return false;
    }


    if (packet.find("find_dialog") != -1) {
        if (packet.find("buttonClicked|spare_btn_") != -1) {
            std::string iID = packet.substr(packet.find("buttonClicked|spare_btn_") + 24, packet.length() - packet.find("buttonClicked|spare_btn_") - 1);
            gameupdatepacket_t xp{ 0 };
            xp.m_type = PACKET_MODIFY_ITEM_INVENTORY;
            xp.m_count = 1;
            xp.m_int_data = atoi(iID.c_str());
            g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&xp, sizeof(gameupdatepacket_t));
            auto item = items[atoi(iID.c_str())];
            gt::send_log("`9succesfully gave `#" + item.name);
            visual_item_id.push_back(xp.m_int_data);
        }
        if (packet.find("buttonClicked|remove_visual_btn_") != -1) {
            std::string remove_visual_id = packet.substr(packet.find("buttonClicked|remove_visual_btn_") + 32, packet.length() - packet.find("buttonClicked|remove_visual_btn_") - 1);
            gameupdatepacket_t xp{ 0 };
            xp.m_type = PACKET_MODIFY_ITEM_INVENTORY;
            xp.m_jump_amount = 1;
            xp.m_int_data = atoi(remove_visual_id.c_str());
            g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&xp, sizeof(gameupdatepacket_t));
            auto item = items[atoi(remove_visual_id.c_str())];
            gt::send_log("`9Succesfully removed `#" + item.name);

            auto item_remove = std::find(visual_item_id.begin(), visual_item_id.end(), xp.m_int_data);
            if (item_remove != visual_item_id.end()) {
                visual_item_id.erase(item_remove);
            }

            /*
            std::vector<float> visual_clothes = { back, pants, shoe, mask, neck, ances, shirt, face, hair, hand };

            auto item_unwear = std::find(visual_clothes.begin(), visual_clothes.end(), xp.m_int_data);
            if (item_unwear != visual_clothes.end()) {
                *item_unwear = 0.f;
            }
            if (events::out::get_punch_id(packet->m_int_data) != 0) {
                punch_effect = events::out::get_punch_id(packet->m_int_data);
            }
            if (back != 0000.0) {
                doublejump = true;
            }
            sendState(world.local.netid);
            variantlist_t liste{ "OnSetClothing" };
            liste[1] = vector3_t{ hair,  shirt,  pants };
            liste[2] = vector3_t{ shoe,  face,  hand };
            liste[3] = vector3_t{ back,  mask,  neck };
            liste[4] = skincolor;
            liste[5] = vector3_t{ ances , 1.f, 0.f };
            g_server->send(true, liste, world.local.netid, -1);
            */
        }
        if (packet.find("vclothes") != -1) {
            try {
                std::string aaa = packet.substr(packet.find("hes|") + 4, packet.size());
                std::string number = aaa.c_str();
                while (!number.empty() && isspace(number[number.size() - 1]))
                    number.erase(number.end() - (76 - 0x4B));
                iswear = stoi(number);
                if (iswear == true) {

                }
            }
            catch (exception a)
            {
                gt::send_log("`4Critical Error: `2override detected");

            }
        }
        return true;
    }

    if (packet.find("buttonClicked|gscan_block") != -1) {
        auto round = [](double n) {
            return n - floor(n) >= 0.5 ? ceil(n) : floor(n);
        };

        std::map<int, std::map<std::string, int>> store1;
        
        for (auto& tile : g_server->m_world.GetTileMap().tiles) {
            if (tile.foreground != 0 || tile.background != 0) {
                if (store1[tile.foreground].empty()) {
                    store1[tile.foreground] = { {"id", tile.foreground}, {"qty", 1} };
                }
                else {
                    store1[tile.foreground]["qty"] += 1;
                }
                if (store1[tile.background].empty()) {
                    store1[tile.background] = { {"id", tile.background}, {"qty", 1} };
                }
                else {
                    store1[tile.background]["qty"] += 1;
                }
            }
        }
        std::string placed_items = "add_spacer|small|";
        for (auto& tile : store1) {
            int count = tile.second["qty"];
            int idplaced = tile.second["id"];
            if (idplaced != 0) placed_items += "\nadd_label_with_icon|small|`0" + std::to_string(count) + "``|left|" + std::to_string(idplaced);
            //cout << (to_string(count) + " == " + std::to_string(idplaced) + "\n");
        }

        std::string paket =
            "set_default_color|`o\nadd_label_with_icon|big|`0World blocks: `0``|left|170\n" + placed_items + "\nadd_quick_exit";
        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = paket;
        g_server->send(true, liste);
        return true;
    }
    if (packet.find("buttonClicked|gscan_float") != -1) {
        vector<pair<int, int>> ditems;
        string purgesexy = "";

        auto wObjs = g_server->m_world.GetObjectMap().objects;
        for (auto& gObj : wObjs)
        {
            bool copy = true;
            for (int i = 0; i < ditems.size(); i++)
            {
                if (ditems.at(i).first == gObj.id)
                {
                    ditems.at(i).second += gObj.count;
                    copy = false;
                }
            }
            if (copy) {
                ditems.push_back(make_pair(gObj.id, gObj.count));
            }
        }

        for (int i = 0; i < ditems.size(); i++) {
            if (ditems.at(i).first == 0 || ditems.at(i).second == 0) continue;
            // gt::send_log("item id: " + std::to_string(ditems.at(i).first) + " - count: " + std::to_string(ditems.at(i).second));
             //purgesexy += "\nadd_label_with_icon|small|" + index[ditems[i].first].name + " : " + to_string(ditems[i].second) + "|left|" + to_string(ditems[i].first) + "|";
            purgesexy += "\nadd_label_with_icon_button|small|" + index[ditems[i].first].name + " : " + to_string(ditems[i].second) + "|left|" + to_string(ditems[i].first) + "|floating_" + to_string(ditems[i].first) + "|noflags|0|0|";
        }
        std::string paket;
        paket =
            "\nadd_label_with_icon|big|Floating Items|left|6016|"
            //"\nadd_textbox|`cFloating Items|left|2480|"
            "\n" + purgesexy + ""
            "\nadd_quick_exit|"
            "\nend_dialog|growscan_dialog|Cancel|Okay|";
        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = paket;
        g_server->send(true, liste);
        return true;
    }

    if (packet.find("visual_trash_item") != -1) {
        int remove_visual_id = visual_trash_id;
        gameupdatepacket_t xp{ 0 };
        xp.m_type = PACKET_MODIFY_ITEM_INVENTORY;
        xp.m_jump_amount = 1;
        xp.m_int_data = remove_visual_id;

        std::unordered_map<std::string, float*> variable_map = {
            {"back", &back},
            {"pants", &pants},
            {"shoe", &shoe},
            {"mask", &mask},
            {"neck", &neck},
            {"ances", &ances},
            {"shirt", &shirt},
            {"face", &face},
            {"hair", &hair},
            {"hand", &hand}
        };

        for (const auto& entry : variable_map) {
            if (*(entry.second) == static_cast<float>(xp.m_int_data)) {
                //std::cout << "Setting " << entry.first << " to 0" << std::endl;
                *(entry.second) = 0.0;
                // Optionally break the loop if you only want to update the first match.
                // break;
            }
        }
        cout << to_string(back) + "\n";
        cout << to_string(pants) + "\n";
        cout << to_string(shoe) + "\n";
        cout << to_string(mask) + "\n";
        cout << to_string(neck) + "\n";
        cout << to_string(ances) + "\n";
        cout << to_string(shirt) + "\n";
        cout << to_string(face) + "\n";
        cout << to_string(hair) + "\n";
        cout << to_string(hand) + "\n";
        if (events::out::get_punch_id(xp.m_int_data) != 0) {
            punch_effect = 8421376;
        }
        if (back == 0000.0) {
            doublejump = false;
        }

        variantlist_t liste{ "OnSetClothing" };
        liste[1] = vector3_t{ hair,  shirt,  pants };
        liste[2] = vector3_t{ shoe,  face,  hand };
        liste[3] = vector3_t{ back,  mask,  neck };
        liste[4] = skincolor;
        liste[5] = vector3_t{ ances , 1.f, 0.f };
        g_server->send(true, liste, world.GetLocalPlayer().netid, -1);

        g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&xp, sizeof(gameupdatepacket_t));
        auto item = items[remove_visual_id];
        gt::send_log("`9Succesfully removed `#" + item.name);

        auto item_remove = std::find(visual_item_id.begin(), visual_item_id.end(), xp.m_int_data);
        if (item_remove != visual_item_id.end()) {
            visual_item_id.erase(item_remove);
        }
        g_server->sendState(world.GetLocalPlayer().netid);
        return true;
    }
    if (packet.find("action|trash") != -1) {
        if (!world.connected)
            return false;
        //gt::send_log(var.get(1).m_values[1]);
        visual_trash_id = atoi(var.get(1).m_values[1].c_str());

        if (g_server->local_player.inventory.getItemCount(visual_trash_id) == 0) {
            std::string paket;
            paket =
                "\nadd_label_with_icon|big|`4Trash `w" + item_map[visual_trash_id].name + "|left|" + to_string(visual_trash_id) + ""
                "\nadd_textbox|`7Click on okay button to trash item `9(Visual)``|left|"
                "\nembed_data|itemID|" + to_string(visual_trash_id) + ""
                "\nend_dialog|visual_trash_item|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
        }
        else {
            return false;
        }
        return true;
    }

    if (packet.find("buttonClicked|weather_") != -1) {
        weatherVar1 = atoi(packet.substr(56).c_str());
        cout << to_string(weatherVar1) + "\n";
        sendVisualWeather(weatherVar1);
        return true;
    }

    if (packet.find("wrench_mode_dialog") != -1) {
        if (packet.find("buttonClicked|wrench_pull") != -1) {
            wrenchMode = "Pull";
            wrenchPullFrame = "staticYellowFrame";
            wrenchKickFrame = "staticBlueFrame";
            wrenchBanFrame = "staticBlueFrame";
            sendWrenchModeDialog();
        }
        if (packet.find("buttonClicked|wrench_kick") != -1) {
            wrenchMode = "Kick";
            wrenchPullFrame = "staticBlueFrame";
            wrenchKickFrame = "staticYellowFrame";
            wrenchBanFrame = "staticBlueFrame";
            sendWrenchModeDialog();
        }
        if (packet.find("buttonClicked|wrench_ban") != -1) {
            wrenchMode = "Ban";
            wrenchPullFrame = "staticBlueFrame";
            wrenchKickFrame = "staticBlueFrame";
            wrenchBanFrame = "staticYellowFrame";
            sendWrenchModeDialog();
        }
        if (packet.find("buttonClicked|wrench_mode") != -1) {
            wrenchModeOnOff = !wrenchModeOnOff;
            if (!wrenchModeOnOff) {
                wrenchMode = "None";
                wrenchPullFrame = "staticBlueFrame";
                wrenchKickFrame = "staticBlueFrame";
                wrenchBanFrame = "staticBlueFrame";
            }
            sendWrenchModeDialog();
        }
        if (packet.find("buttonClicked|wrench_right_kick") != -1) {
            wrenchRightKick = !wrenchRightKick;
            sendWrenchModeDialog();
        }
        return true;
    }
    if (packet.find("world_option_dialog") != -1) {
        if (packet.find("buttonClicked|world_kick_all") != -1) {
            worldOption("kick");
        }
        if (packet.find("buttonClicked|world_ban_all") != -1) {
            worldOption("ban");
        }
        if (packet.find("buttonClicked|world_pull_all") != -1) {
            worldOption("pull");
        }
        if (packet.find("buttonClicked|world_uba") != -1) {
            g_server->send(false, "action|input\n|text|/uba");
        }
        return true;
    }
    if (packet.find("auto_wrench_dialog") != -1) {
        std::string autopulas = packet.substr(packet.find("auto_pull_name|") + 15, packet.length() - packet.find("auto_pull_name|") - 1);
        std::string basik = " ";
        autoPullName = basik + autopulas;

        if (packet.find("buttonClicked|auto_add_to_list") != -1) {
            sendAutoWrenchDialog();
        }

        return true;
    }

    if (packet.find("autofish_dialog") != -1) {
        baitItemId = atoi(packet.substr(76).c_str());
        gt::send_log("`9Auto Fish Bait: `w" + item_map[baitItemId].name);
        return true;
    }

    if (packet.find("gems_option_dialog") != -1) {
        if (packet.find("collected_gems|") != -1) {
            try {
                std::string aaa = packet.substr(packet.find("collected_gems|") + 15, packet.size());
                std::string number = aaa.c_str();
                if (!collected_gems == stoi(number)) {
                    collected_gems = stoi(number);
                    if (collected_gems) {
                        gt::send_log("`9[GEMS]`w: `2ENABLED `$Show Collect Gems Amount");
                        if (!showGemsCollected_running) showGemsCollected();
                    }
                    else gt::send_log("`9[GEMS]`w: `4DISABLED `$Show Collect Gems Amount");
                }
            }
            catch (exception a)
            {
                gt::send_log("`4Critical Error: `2override detected");

            }
        }
        if (packet.find("instant_gems|") != -1) {
            try {
                std::string aaa = packet.substr(packet.find("instant_gems|") + 13, packet.size());
                std::string number = aaa.c_str();
                if (!instant_gems == stoi(number)) {
                    instant_gems = stoi(number);
                    if (instant_gems) {
                        gt::send_log("`9[GEMS]`w: `2ENABLED `$Show Instant Gems Amount");
                        if (!showGemsInstantDrop_running) showGemsInstantDrop();
                    }
                    else gt::send_log("`9[GEMS]`w: `4DISABLED `$Show Instant Gems Amount");
                }
            }
            catch (exception a)
            {
                gt::send_log("`4Critical Error: `2override detected");

            }
        }
        if (packet.find("punch_tile_gems|") != -1) {
            try {
                std::string aaa = packet.substr(packet.find("punch_tile_gems|") + 16, packet.size());
                std::string number = aaa.c_str();
                if (!punch_tile_gems == stoi(number)) {
                    punch_tile_gems = stoi(number);
                    if (punch_tile_gems) {
                        gt::send_log("`9[GEMS]`w: `2ENABLED `$Show Gems Amount (Punch Tile)");
                    }
                    else gt::send_log("`9[GEMS]`w: `4DISABLED `$Show Gems Amount (Punch Tile)");
                }
            }
            catch (exception a)
            {
                gt::send_log("`4Critical Error: `2override detected");
            }
        }
        return true;
    }

    if (packet.find("add_account_button") != -1) {
        std::string paket;
        paket =
            "\nadd_label_with_icon|big|Add Account|left|8192|"
            "\nadd_spacer|small"
            "\nadd_text_input|growid_text_input|GrowId: ||20|"
            "\nadd_text_input|password_text_input|Password: ||20|"
            "\nadd_quick_exit|"
            "\nend_dialog|add_account_dialog|Cancel|Okay|";
        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = paket;
        g_server->send(true, liste);
        return true;
    }

    if (packet.find("buttonClicked|growid") != -1) {
        std::string growidnumber = packet.substr(packet.find("buttonClicked|growid") + 20, packet.find("buttonClicked|growid") - packet.find("buttonClicked|growid") - 20);
        std::cout << "Clicked growid: " << growidnumber << std::endl;

        std::ifstream infile("name_password.txt");
        if (infile.is_open()) {
            std::string line;
            int currentGrowid = 1;
            while (std::getline(infile, line)) {
                size_t pos = line.find(":");
                if (pos != -1) {
                    std::string name = line.substr(0, pos);
                    std::string password = line.substr(pos + 1);
                    if (currentGrowid == std::stoi(growidnumber)) {
                        //std::cout << "Name: " << name << ", Password: " << password << std::endl;

                        switchAccount = true;
                        switchGrowid = name;
                        switchGrowidPass = password;
                        cout << "GrowID: " << switchGrowid << endl;
                        cout << "Pass: " << switchGrowidPass << endl;
                        gt::send_log("`9Logging in to: `#" + name);
                        variantlist_t list{ "OnReconnect" };
                        g_server->send(true, list);
                        break;
                    }
                }
                currentGrowid++;
            }
            infile.close();
        }
        else {
            std::cerr << "Failed to open file for reading.\n";
        }
    }


    if (packet.find("add_account_dialog") != -1) {
        std::string name = packet.substr(packet.find("growid_text_input|") + 18, packet.find("password_text_input|") - packet.find("growid_text_input|") - 19);
        std::string password = packet.substr(packet.find("password_text_input|") + 20);
        writeAcc(name, password);
    }

    if (packet.find("login_default_growid") != -1) {

        if (defaultGrowId == removeColors(g_server->m_world.GetLocalPlayer().name)) {
            gt::send_log("`4You are already logged in with this GrowID");
        }
        else {
            gt::send_log("`9Logging in to: `#");
            variantlist_t list{ "OnReconnect" };
            g_server->send(true, list);
            //g_server->send(false, "action|quit", NET_MESSAGE_GAME_MESSAGE);
            //g_server->quit();
            //this_thread::sleep_for(std::chrono::milliseconds(500));
            //g_server->send(false, "action|enter_game", NET_MESSAGE_GENERIC_TEXT);
        }



        return true;
    }
   
    if (wrenchModeOnOff) {
        if (wrenchRightKick == true) {
            if (packet.find("action|wrench") != -1) {
                if (GetAsyncKeyState(0x02)) {
                    g_server->send(false, packet);
                    std::string str = packet.substr(packet.find("netid|") + 6, packet.length() - packet.find("netid|") - 1);
                    std::string wrenchNetid = str.substr(0, str.find("|"));
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + wrenchNetid + "|\nnetID|" + wrenchNetid + "|\nbuttonClicked|kick");
                    
                    variantlist_t varlist{ "OnTextOverlay" };
                    varlist[1] = "`9Successfuly " + mode + " `#" + wrenchNetid;
                    g_server->send(true, varlist);
                    
                    return true;
                }
            }
        }
        if (packet.find("action|wrench") != -1) {
            g_server->send(false, packet);

            std::string str = packet.substr(packet.find("netid|") + 6, packet.length() - packet.find("netid|") - 1);
            std::string wrenchNetid = str.substr(0, str.find("|"));
            if (wrenchMode == "Pull") {
                g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + wrenchNetid + "|\nnetID|" + wrenchNetid + "|\nbuttonClicked|pull");
            }
            if (wrenchMode == "Kick") {
                g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + wrenchNetid + "|\nnetID|" + wrenchNetid + "|\nbuttonClicked|kick");
            }
            if (wrenchMode == "Ban") {
                g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + wrenchNetid + "|\nnetID|" + wrenchNetid + "|\nbuttonClicked|worldban");
            }
            variantlist_t varlist{ "OnTextOverlay" };
            varlist[1] = "`9Successfuly " + mode + " `#" + wrenchNetid;
            g_server->send(true, varlist);

            return true;
        }
    }

    if (var.get(0).m_key == "action" && var.get(0).m_value == "input") {
        if (var.size() < 2)
            return false;
        if (var.get(1).m_values.size() < 2)
            return false;
        if (!world.connected)
            return false;
        auto chat = var.get(1).m_values[1]; //changed auto to string

        if (find_command(chat, "find ")) {
            if (iswear == true) {
                bc = "1";
            }
            else {
                bc = "0";
            }
            std::string hypercold = chat.substr(6);
            string find_list = "";

            for (int i = 0; i < index.size(); i++) {
                item_id = index[i].itemID;
                if (to_lower(index[item_id].name).find(hypercold) != string::npos) {
                    if (index[item_id].name.find(" Seed") != std::string::npos) continue;
                    //gt::send_log("" + to_string(item_id) + ": " + index[item_id].name + "");
                    //find_list += "\nadd_label_with_icon|small|" + to_string(item_id) + " -> " + index[item_id].name + "|left|" + to_string(item_id) + "|";
                    find_list += "\nadd_label_with_icon_button|small|" + to_string(item_id) + " -> " + index[item_id].name + "|left|" + to_string(item_id) + "|spare_btn_" + to_string(item_id) + "|noflags|0|0|";
                }
            }

            std::string paket;
            paket =
                "\nadd_label_with_icon|big|Search Results For `c" + hypercold + "|left|6016|"
                "\nadd_textbox|`2Click on item icon to add it to your inventory|left|2480|"
                "\nadd_checkbox|vclothes|`cEnable Visual Clothes|" +
                bc +
                "|"
                "\n" + find_list + ""
                "\nadd_quick_exit|"
                "\nend_dialog|find_dialog|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);

            return true;
        }

        else if (find_command(chat, "name ")) {
            std::string name = "``" + chat.substr(6) + "``";
            variantlist_t va{ "OnNameChanged" };
            va[1] = name;
            g_server->m_world.GetLocalPlayer().name = name;
            //visualName = name;
            g_server->send(true, va, world.GetLocalPlayer().netid, -1);
            gt::send_log("`#Name set to : " + name);
            return true;
        }

        /*else if (find_command(chat, "farm")) {
            auto_farm = !auto_farm;
            if (auto_farm) {
                gt::send_log("START");
                std::thread(dofarm).detach();
            }
            else {
                gt::send_log("STOP");
            }
            return true;
        }*/



        else if (find_command(chat, "wrench")) {
            sendWrenchModeDialog();
            return true;
        }
        else if (find_command(chat, "auto")) {
            sendAutoWrenchDialog();
            return true;
        }
        else if (find_command(chat, "world")) {
            std::string paket;
            paket =
                "\nadd_label_with_icon|big|World Commands|left|32|"
                "\nadd_spacer|small"
                "\nadd_button|world_kick_all|`4Kick All``|noflags|0|0|"
                "\nadd_button|world_ban_all|`4Ban All``|noflags|0|0|"
                "\nadd_button|world_pull_all|`5Pull All``|noflags|0|0|"
                "\nadd_button|world_uba|`5Unban Banned Player in World``|noflags|0|0|"
                "\nadd_quick_exit|"
                "\nend_dialog|world_option_dialog||Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
        }



        else if (find_command(chat, "autofish")) {
            string baitname = "";
            if (baitItemId == 0) baitname = "None";
            else baitname = item_map[baitItemId].name;

            std::string paket;
            paket =
                "\nadd_label_with_icon|big|`9Auto Fish Page|left|32|"
                "\nembed_data|tilex|15"
                "\nembed_data|tiley|23"
                "\nadd_spacer|small|"
                "\nadd_textbox|`5Selected Bait: `w" + baitname + "|left|"
                "\nadd_item_picker|itemid|Select Bait|Choose the bait that you want to use|"
                "\nadd_spacer|small|"
                "\nadd_smalltext|Type `2/fish `wto enable autofish and throw bait then AFK|"
                "\nadd_smalltext|Type `2/fish `wagain to disable autofish|"
                "\nend_dialog|autofish_dialog|`0Cancel|`0Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
        }

        else if (find_command(chat, "fish")) {
            if (baitItemId == 0) {
                gt::send_log("`9Select Bait First using `2/autofish `9command");
            }
            else if (g_server->local_player.inventory.getItemCount(baitItemId) == 0) {
                gt::send_log("`9You do not have enough bait in your backpack.");
            }
            else {
                startAutoFish = !startAutoFish;
                if (startAutoFish) gt::send_log("`9Auto Fish `2Enabled");
                else gt::send_log("`9Auto Fish `4Disabled");
            }
            return true;
        }



        else if (find_command(chat, "balance")) {
            gt::send_log("CDROP: " + to_string(balance()));
            gt::send_log("`2WL : " + to_string(item_count(242)));
            gt::send_log("`2DL : " + to_string(item_count(1796)));
            gt::send_log("`2BGL : " + to_string(item_count(7188)));
            return true;
        }
         
        else if (find_command(chat, "vend")) {
            string venddialog = "";



            for (auto& tile : g_server->m_world.GetTileMap().tiles) {
                if (tile.foreground == 2978 ) {
                    cout << "\nItem ID: " + to_string(tile.tileExtra.vending.item_id) + " Item Price: " + to_string(tile.tileExtra.vending.price);
                    venddialog += "\nadd_label_with_icon_button|small|" + index[tile.tileExtra.vending.item_id].name + " : " + to_string(tile.tileExtra.vending.item_id) + "|left|" + to_string(tile.tileExtra.vending.item_id) + "|floating_" + to_string(tile.tileExtra.vending.item_id) + "|noflags|0|0|";
                }
            }
            cout << "\n";
/*
            for (auto& tile : g_server->m_world.GetTileMap().tiles2) {
                //cout << "yes\n";
                tile.second.tileExtra.
                //cout << to_string(tile.second.foreground) + "\n";
            }
*/
            std::string paket;
            paket =
                "\nadd_label_with_icon|big|Floating Items|left|6016|"
                //"\nadd_textbox|`cFloating Items|left|2480|"
                "\n" + venddialog + ""
                "\nadd_quick_exit|"
                "\nend_dialog|growscan_dialog|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
        }

        else if (find_command(chat, "autocollect")) {
            std::string paket;
            paket =
                "\nadd_label_with_icon|big|Auto Collect Settings|left|112|"
                "\nadd_spacer|small"
                "\nadd_checkbox|autocollect_toggle|`cEnable Auto Collect|1|"

                "\nadd_custom_margin|x:0;y:-45|"
                "\nadd_custom_textbox|                  `^>> Shortcut Command: `9/ac|size:tiny|"
                "\nadd_custom_margin|x:0;y:10|"
                "\nadd_spacer|small"

                /*
                "\nadd_checkbox|autocollectaura_toggle|`cShow Auto Collect Range Effect|1|"
                "\nadd_custom_margin|x:0;y:-45|"
                "\nadd_custom_textbox|                  `^>> Shows you auto collect range|size:tiny|"
                "\nadd_custom_margin|x:0;y:10|"
                "\nadd_spacer|small"
                */

                "\nadd_text_input|autocollect_range|Collect Range: |5|4|"
                "\nadd_custom_textbox|`^  >> Max auto collect range is `910|size:tiny|"
                "\nadd_quick_exit|"
                "\nend_dialog|autocollect_dialog|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
        }
        

        
        else if (find_command(chat, "ceffect")) {
            showAutoCollectEffect = !showAutoCollectEffect;
            thread(showAutoCollectEffectArea).detach();
            if (showAutoCollectEffect) {
                gt::send_log("`9[AC]`w: `2ENABLED `$Auto Collect");
            }
            else gt::send_log("`9[AC]`w: `4DISABLED `$Auto Collect");
            return true;
        }

        else if (find_command(chat, "ac")) {
            autoCollect = !autoCollect;
            if (autoCollect) {
                thread(doAutoCollect).detach();
                gt::send_log("`9[AC]`w: `2ENABLED `$Auto Collect");
            }
            else gt::send_log("`9[AC]`w: `4DISABLED `$Auto Collect");
            return true;
        }

        else if (find_command(chat, "farm")) {
            auto_farm = !auto_farm;
            if (auto_farm) {
                thread(doAutofarm, 2, (g_server->m_world.GetLocalPlayer().pos.m_x / 32) + 1, (g_server->m_world.GetLocalPlayer().pos.m_y / 32)).detach();
                gt::send_log("AF ON");
            }
            else gt::send_log("AF OFF");
            return true;
        }

        else if (find_command(chat, "cd ")) {
            try {
                std::string cdropcount = chat.substr(4);
                int sayi = stoi(cdropcount);

                if (balance() < sayi) {
                    gt::send_log("`4You don't have enough `9World Lock!");
                    return true;
                }
                if (sayi < 100) {

                    if (item_count(242) < sayi) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 1796;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }

                    dropwl = true;
                    g_server->send(false, "action|drop\n|itemID|242");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));
                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + cdropcount); //242
                    gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
                }

                else if (sayi > 10000) {


                    int sayi1 = (sayi / 10000);

                    int kalan = ((sayi / 100) - (sayi1 * 100));
                    int kalan2 = sayi - ((kalan * 100) + (sayi1 * 10000));
                    if (kalan > item_count(1796)) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 7188;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    else if (item_count(242) < kalan2) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 1796;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    dropbgl = true;
                    g_server->send(false, "action|drop\n|itemID|7188");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|7188|\ncount|" + std::to_string(sayi1)); //242

                    dropdl = true;
                    g_server->send(false, "action|drop\n|itemID|1796");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string(kalan)); //242

                    dropwl = true;
                    g_server->send(false, "action|drop\n|itemID|242");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + std::to_string(kalan2)); //242

                    gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
                }
                else {
                    int sayi1 = (sayi / 100);
                    int kalan = (sayi % 100);

                    if (item_count(242) < kalan) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 1796;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));

                    }
                    else if (item_count(1796) < sayi1) {
                        gameupdatepacket_t drop{ 0 };
                        drop.m_type = PACKET_ITEM_ACTIVATE_REQUEST;
                        drop.m_int_data = 242;
                        g_server->send(false, NET_MESSAGE_GAME_PACKET, (uint8_t*)&drop, sizeof(gameupdatepacket_t));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));

                    }
                    dropdl = true;
                    g_server->send(false, "action|drop\n|itemID|1796");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + std::to_string(sayi1)); //242

                    dropwl = true;
                    g_server->send(false, "action|drop\n|itemID|242");
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));

                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + std::to_string(kalan)); //242

                    gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
                }
            }
            catch (std::exception) { gt::send_log("Critical Error : Invalid String Position"); }
            return true;
            }

        else if (find_command(chat, "tp")) {
            game_started = true;

            bool bets_are_equal = (scanwldrop(autohost_pos1) == scanwldrop(autohost_pos2));
            if (scanwldrop(autohost_pos3) != 0) {
                bets_are_equal &= (scanwldrop(autohost_pos1) == scanwldrop(autohost_pos3));
                bets_are_equal &= (scanwldrop(autohost_pos2) == scanwldrop(autohost_pos3));
            }
            if (scanwldrop(autohost_pos4) != 0) {
                bets_are_equal &= (scanwldrop(autohost_pos1) == scanwldrop(autohost_pos4));
                bets_are_equal &= (scanwldrop(autohost_pos2) == scanwldrop(autohost_pos4));
                bets_are_equal &= (scanwldrop(autohost_pos3) == scanwldrop(autohost_pos4));
            }

            if (bets_are_equal) {
                gt::send_log("BETS ARE EQUAL");
                if (autohost_pos4.m_x != 0) {
                    total_bet = scanwldrop(autohost_pos1) * 4;
                }
                else if (autohost_pos3.m_x != 0) {
                    total_bet = scanwldrop(autohost_pos1) * 3;
                }
                else {
                    total_bet = scanwldrop(autohost_pos1) * 2;
                }
                gt::send_log("Total Bet: " + to_string(total_bet));
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                autohost();
            }
            else {
                gt::send_log("BETS ARE NOT EQUAL");
            }
            return true;
        }

        /*
        else if (find_command(chat, "tp")) {
            game_started = true;

            if (scanwldrop(autohost_pos1) == scanwldrop(autohost_pos2)) {
                gt::send_log("BETS ARE EQUAL");
                total_bet = scanwldrop(autohost_pos1) * 2;
                gt::send_log("Total Bet: " + to_string(total_bet));
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                autohost();
                return true;
            }
            else {
                gt::send_log("BETS ARE NOT EQUAL");
                return true;
            }
        }
        */
/*
        else if (find_command(chat, "tp")) {
            game_started = true;

            auto& bruh = g_server->m_world.GetLocalPlayer();
            float playerx = bruh.pos.m_x;
            float playery = bruh.pos.m_y;
            host::startlocx = to_string(playerx);
            host::startlocy = to_string(playery);
            pos1.m_x = std::atoi(host::pos1x.c_str());
            pos1.m_y = std::atoi(host::pos1y.c_str());
            pos2.m_x = std::atoi(host::pos2x.c_str());
            pos2.m_y = std::atoi(host::pos2y.c_str());
            int p1 = pos1.m_x / 32;
            int p2 = pos1.m_y / 32;

            if (scanwldrop(pos1.m_x / 32, pos1.m_y /32) == scanwldrop(pos2.m_x / 32, pos2.m_y / 32)) {
                gt::send_log("BETS ARE EQUAL");
                total_bet = scanwldrop(pos1.m_x / 32, pos1.m_y / 32) * 2;
                gt::send_log("Total Bet: " + to_string(total_bet));
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                autohost();
                return true;
            }
            else {
                gt::send_log("BETS ARE NOT EQUAL");
                return true;
            }
        }
*/

        //Casino Option
        else if (find_command(chat, "autohost")) {

            return true;
        }

        else if (find_command(chat, "pos1")) {
            auto& local = g_server->m_world.GetLocalPlayer();
            autohost_pos1.m_x = local.pos.m_x;
            autohost_pos1.m_y = local.pos.m_y;

            variantlist_t varlist{ "OnParticleEffect" };
            varlist[1] = 88;
            varlist[2] = vector2_t{ local.pos.m_x + 10,  local.pos.m_y + 15 };
            varlist[3] = 0;
            varlist[4] = 0;
            g_server->send(true, varlist);

            gt::send_log("`91st Position: `#" + to_string((int)autohost_pos1.m_x / 32) + ", " + to_string((int)autohost_pos1.m_y / 32));
            return true;
        }
        else if (find_command(chat, "pos2")) {
            if (autohost_pos1.m_x == 0) {
                gt::send_log("Set Pos 1 First");
                return true;
            }

            auto& local = g_server->m_world.GetLocalPlayer();
            autohost_pos2.m_x = local.pos.m_x;
            autohost_pos2.m_y = local.pos.m_y;
            variantlist_t varlist{ "OnParticleEffect" };
            varlist[1] = 88;
            varlist[2] = vector2_t{ local.pos.m_x + 10,  local.pos.m_y + 15 };
            varlist[3] = 0;
            varlist[4] = 0;
            g_server->send(true, varlist);
            gt::send_log("`92nd Position: `#" + to_string((int)autohost_pos2.m_x / 32) + ", " + to_string((int)autohost_pos2.m_y / 32));
            return true;
        }
        else if (find_command(chat, "pos3")) {
            if (autohost_pos2.m_x == 0) {
                gt::send_log("Set Pos 2 First");
                return true;
            }
                auto& local = g_server->m_world.GetLocalPlayer();
                autohost_pos3.m_x = local.pos.m_x;
                autohost_pos3.m_y = local.pos.m_y;
                variantlist_t varlist{ "OnParticleEffect" };
                varlist[1] = 88;
                varlist[2] = vector2_t{ local.pos.m_x + 10,  local.pos.m_y + 15 };
                varlist[3] = 0;
                varlist[4] = 0;
                g_server->send(true, varlist);
                gt::send_log("`93rd Position: `#" + to_string((int)autohost_pos3.m_x / 32) + ", " + to_string((int)autohost_pos3.m_y / 32));
                return true;
        }
        else if (find_command(chat, "pos4")) {
            if (autohost_pos3.m_x == 0) {
                gt::send_log("Set Pos 3 First");
                return true;
            }
                    auto& local = g_server->m_world.GetLocalPlayer();
                    autohost_pos4.m_x = local.pos.m_x;
                    autohost_pos4.m_y = local.pos.m_y;
                    variantlist_t varlist{ "OnParticleEffect" };
                    varlist[1] = 88;
                    varlist[2] = vector2_t{ local.pos.m_x + 10,  local.pos.m_y + 15 };
                    varlist[3] = 0;
                    varlist[4] = 0;
                    g_server->send(true, varlist);
                    gt::send_log("`94t Position: `#" + to_string((int)autohost_pos4.m_x / 32) + ", " + to_string((int)autohost_pos4.m_y / 32));
                    return true;
        }
        else if (find_command(chat, "rpos")) {
                        autohost_pos1.m_x = 0;
                        autohost_pos1.m_y = 0;
                        autohost_pos2.m_x = 0;
                        autohost_pos2.m_y = 0;
                        autohost_pos3.m_x = 0;
                        autohost_pos3.m_y = 0;
                        autohost_pos4.m_x = 0;
                        autohost_pos4.m_y = 0;
                        gt::send_log("Removed Auto Host Pos");
                        return true;
                        }
        else if (find_command(chat, "cpos")) {
            if (autohost_pos1.m_x != 0) {
                variantlist_t varlist{ "OnParticleEffect" };
                varlist[1] = 181;
                varlist[2] = vector2_t{ float((autohost_pos1.m_x ) + 16), float(float((autohost_pos1.m_y) + 14))};
                varlist[3] = float(1);
                varlist[4] = float(0.000000);
                g_server->send(true, varlist);
            }
            if (autohost_pos2.m_x != 0) {
                variantlist_t varlist{ "OnParticleEffect" };
                varlist[1] = 181;
                varlist[2] = vector2_t{ float((autohost_pos2.m_x) + 16), float(float((autohost_pos2.m_y) + 14)) };
                varlist[3] = float(2);
                varlist[4] = float(0.000000);
                g_server->send(true, varlist);
            }
            if (autohost_pos3.m_x != 0) {
                variantlist_t varlist{ "OnParticleEffect" };
                varlist[1] = 181;
                varlist[2] = vector2_t{ float((autohost_pos3.m_x) + 16), float(float((autohost_pos3.m_y) + 14)) };
                varlist[3] = float(3);
                varlist[4] = float(0.000000);
                g_server->send(true, varlist);
            }
            if (autohost_pos4.m_x != 0) {
                variantlist_t varlist{ "OnParticleEffect" };
                varlist[1] = 181;
                varlist[2] = vector2_t{ float((autohost_pos4.m_x) + 16), float(float((autohost_pos4.m_y) + 14)) };
                varlist[3] = float(4);
                varlist[4] = float(0.000000);
                g_server->send(true, varlist);
            }
            return true;
        }

        else if (find_command(chat, "win1")) {
            if (autohost_pos1.m_x == 0) return true;
            vector2_t pos;
            pos.m_x = autohost_pos1.m_x;
            pos.m_y = autohost_pos1.m_y;
            int normalx = autohost_pos1.m_x / 32;
            int normaly = autohost_pos1.m_y / 32;
            gt::findpath(normalx, normaly);
            if (notax_mode == true) {
                gt::send_log("Total Bet: " + to_string(total_bet));
                bool aga = custom_drop(total_bet, pos, autohost_pos1.m_x, autohost_pos1.m_y);
            }
            else {
                bool aga = custom_drop((total_bet - (total_bet / 10)), pos, autohost_pos1.m_x, autohost_pos1.m_y);
            }
            game_started = false;
            return true;
        }
        else if (find_command(chat, "win2")) {
            if (autohost_pos2.m_x == 0) return true;
            vector2_t pos;
            pos.m_x = autohost_pos2.m_x;
            pos.m_y = autohost_pos2.m_y;
            int normalx = autohost_pos2.m_x / 32;
            int normaly = autohost_pos2.m_y / 32;
            gt::findpath(normalx, normaly);
            if (notax_mode == true) {
                gt::send_log("Total Bet: " + to_string(total_bet));
                bool aga = custom_drop(total_bet, pos, autohost_pos2.m_x, autohost_pos2.m_y);
            }
            else {
                bool aga = custom_drop((total_bet - (total_bet / 10)), pos, autohost_pos2.m_x, autohost_pos2.m_y);
            }
            game_started = false;
            return true;
        }
        else if (find_command(chat, "win3")) {
            if (autohost_pos3.m_x == 0) return true;
            vector2_t pos;
            pos.m_x = autohost_pos3.m_x;
            pos.m_y = autohost_pos3.m_y;
            int normalx = autohost_pos3.m_x / 32;
            int normaly = autohost_pos3.m_y / 32;
            gt::findpath(normalx, normaly);
            if (notax_mode == true) {
                gt::send_log("Total Bet: " + to_string(total_bet));
                bool aga = custom_drop(total_bet, pos, autohost_pos3.m_x, autohost_pos3.m_y);
            }
            else {
                bool aga = custom_drop((total_bet - (total_bet / 10)), pos, autohost_pos3.m_x, autohost_pos3.m_y);
            }
            game_started = false;
            return true;
        }
        else if (find_command(chat, "win4")) {
            if (autohost_pos4.m_x == 0) return true;
            vector2_t pos;
            pos.m_x = autohost_pos4.m_x;
            pos.m_y = autohost_pos4.m_y;
            int normalx = autohost_pos4.m_x / 32;
            int normaly = autohost_pos4.m_y / 32;
            gt::findpath(normalx, normaly);
            if (notax_mode == true) {
                gt::send_log("Total Bet: " + to_string(total_bet));
                bool aga = custom_drop(total_bet, pos, autohost_pos4.m_x, autohost_pos4.m_y);
            }
            else {
                bool aga = custom_drop((total_bet - (total_bet / 10)), pos, autohost_pos4.m_x, autohost_pos4.m_y);
            }
            game_started = false;
            return true;
        }


        
        //Gems Options
        else if (find_command(chat, "sgc")) {
            collected_gems = !collected_gems;
            if (collected_gems) {
                gt::send_log("`9[GEMS]`w: `2ENABLED `$Show Collect Gems Amount");
                if (!showGemsCollected_running) showGemsCollected();
            }
            else gt::send_log("`9[GEMS]`w: `4DISABLED `$Show Collect Gems Amount");


            return true;
        }
        else if (find_command(chat, "sgd")) {
            instant_gems = !instant_gems;
            if (instant_gems) {
                gt::send_log("`9[GEMS]`w: `2ENABLED `$Show Instant Gems Amount");
                if (!showGemsInstantDrop_running) showGemsInstantDrop();
            }
            else gt::send_log("`9[GEMS]`w: `4DISABLED `$Show Instant Gems Amount");
            return true;
        }
        else if (find_command(chat, "sgp")) {
            punch_tile_gems = !punch_tile_gems;
            if (punch_tile_gems) {
                gt::send_log("`9[GEMS]`w: `2ENABLED `$Show Gems Amount (Punch Tile)");
            }
            else gt::send_log("`9[GEMS]`w: `4DISABLED `$Show Gems Amount (Punch Tile)");
            return true;
        }
        else if (find_command(chat, "dgems")) {
            std::map<std::pair<int, int>, int> positionCountMap;
            auto round = [](double n) {
                return n - floor(n) >= 0.5 ? ceil(n) : floor(n);
            };
            for (auto& obj : g_server->m_world.GetObjectMap().objects) {
                if (obj.id == 112) {
                    int posX = ((obj.pos.X + 10) / 32);
                    int posY = ((obj.pos.Y + 10) / 32);

                    totalGemsInSpecificTiles(posX, posY);
                }
            }
            return true;
        }
        else if (find_command(chat, "gems")) {
            std::string paket;
            paket =
                "\nadd_label_with_icon|big|Gems Options|left|112|"
                "\nadd_spacer|small"

                "\nadd_checkbox|collected_gems|`^Enable `#Show Collected Gems Amount|" + to_string(collected_gems ? 1 : 0) + "|"
                "\nadd_custom_margin|x:0;y:-45|"
                "\nadd_custom_textbox|                  `2> `7Show gems amount that you collect `9(Shortcut Command: /sgc)|size:tiny|"
                "\nadd_custom_margin|x:0;y:10|"

                "\nadd_spacer|small"

                "\nadd_checkbox|instant_gems|`^Enable `#Show Instant Gems Drop|" + to_string(instant_gems ? 1 : 0) + "|"
                "\nadd_custom_margin|x:0;y:-45|"
                "\nadd_custom_textbox|                  `2> `7Show gems amount instantly when gems are dropped `9(Shortcut Command: /sgd)|size:tiny|"
                "\nadd_custom_margin|x:0;y:10|"

                "\nadd_spacer|small"

                "\nadd_checkbox|punch_tile_gems|`^Enable `#Show Gems When Punch Tile|" + to_string(punch_tile_gems ? 1 : 0) + "|"
                "\nadd_custom_margin|x:0;y:-45|"
                "\nadd_custom_textbox|                  `2> `7Show gems amount when you punch tile `9(Shortcut Command: /sgp)|size:tiny|"
                "\nadd_custom_margin|x:0;y:10|"

                "\nadd_quick_exit|"
                "\nend_dialog|gems_option_dialog|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
        }



        else if (find_command(chat, "ping")) {
            showLatencyPing = !showLatencyPing;
            if (showLatencyPing) gt::send_log("`9[PING]`w: `2ENABLED `$Show Latency Ping");
            else {
                gt::send_log("`9[PING]`w: `4DISABLED `$Show Latency Ping");
                variantlist_t va{ "OnNameChanged" };
                va[1] = g_server->m_world.GetLocalPlayer().name;
                g_server->send(true, va, g_server->m_world.GetLocalPlayer().netid, -1);
            }
            return true;
        }


        else if (find_command(chat, "warp ")) {
            g_server->send(false, "action|join_request\nname|" + chat.substr(5) + "\ninvitedWorld|0", 3);
            gt::send_log("`9Warping to `#" + chat.substr(5));
            return true;
        }

        else if (find_command(chat, "relog")) {
            string ruzgar = g_server->m_world.GetName();
            g_server->send(false, "action|quit_to_exit", 3);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            g_server->send(false, "action|join_request\nname|" + ruzgar + "\ninvitedWorld|0", 3);
            gt::send_log("`9Relog World!");
            return true;
        }

        else if (find_command(chat, "back")) {
            string ruzgar = g_server->m_world.GetName();
            g_server->send(false, "action|join_request\nname|" + old_world + "\ninvitedWorld|0", 3);
            gt::send_log("`9Warps to `#previously `9entered world!");
            return true;
            //g_server->send(false, "action|quit_to_exit", 3);
        }

        else if (find_command(chat, "debug"))
        {
            debug = !debug;
            if (debug)
                gt::send_log("`9Packet Debugger Mode is `2ON");
            else
                gt::send_log("`9Packet Debugger Mode is `4OFF");
            return true;
        }

        else if (find_command(chat, "collect")) {
            gt::send_log("`9Collecting items in 4 far");
            for (auto it = g_server->m_world.GetObjectMap().objects.begin(); it != g_server->m_world.GetObjectMap().objects.end(); ++it)
            {
                if (isInside(it->pos.X, it->pos.Y, 4 * 32, g_server->m_world.GetLocalPlayer().pos.m_x, g_server->m_world.GetLocalPlayer().pos.m_y))
                {
                    GameUpdatePacket packet{ 0 };
                    packet.pos_x = it->pos.X;
                    packet.pos_y = it->pos.Y;
                    packet.type = 11;
                    packet.netid = -1;
                    packet.object_id = it->offset;
                    g_server->send(false, 4, (uint8_t*)&packet, sizeof(GameUpdatePacket));
                }
            }
            return true;
        }

        else if (find_command(chat, "nightvision") || find_command(chat, "nv")) {
            for (auto& tile : g_server->m_world.GetTileMap().tiles) {
                if (tile.background == 3556) {
                    GameUpdatePacket packet{ 0 };
                    packet.type = PACKET_TILE_CHANGE_REQUEST;
                    packet.item_id = 56;
                    packet.int_x = tile.position.X;
                    packet.int_y = tile.position.Y;
                    g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(GameUpdatePacket));
                }
            }
            return true;
        }

        else if (find_command(chat, "antigravity") || find_command(chat, "ag")) {
            GameUpdatePacket packet{ 0 };
            packet.type = PACKET_TILE_CHANGE_REQUEST;
            packet.item_id = 4992;
            packet.int_x = 99;
            packet.int_y = 59;
            g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&packet, sizeof(GameUpdatePacket));
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            auto pos = g_server->m_world.GetLocalPlayer().pos;

            g_server->enterDoor(true, 99, 59);

            GameUpdatePacket legitpacket{ 0 };
            legitpacket.type = PACKET_STATE;
            legitpacket.item_id = 18;
            legitpacket.int_x = 99;
            legitpacket.int_y = 59;
            legitpacket.vec_x = pos.m_x;
            legitpacket.vec_y = pos.m_y;
            legitpacket.flags = 2592;
            g_server->send(true, NET_MESSAGE_GAME_PACKET, (uint8_t*)&legitpacket, sizeof(GameUpdatePacket));
            antigravity = true;
            return true;
        }

        else if (find_command(chat, "growscan") || find_command(chat, "gscan") || find_command(chat, "gs")) {
            std::string paket;
            paket =
                "\nset_default_color|`o"
                "\nadd_label_with_icon|big|`9Growscan|left|6016|"
                "\nadd_spacer|small|"
                "\nadd_button|gscan_float|`9Floating items``|noflags|0|0|"
                "\nadd_button|gscan_block|`9Word blocks``|noflags|0|0|"
                "\nadd_spacer|small|"
                "\nadd_quick_exit|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
        }

        else if (find_command(chat, "pf")) {
            pf = !pf;
            if (pf) gt::send_log("Pathfind On");
            else gt::send_log("Pathfind Off");

            std::thread([&]() {
                while (pf) {
                    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                        if (!shiftPressed) {
                            g_server->longpunch(g_server->m_world.GetLocalPlayer().netid, true);
                            shiftPressed = true;
                        }
                    }
                    else {
                        if (shiftPressed) {
                            g_server->longpunch(g_server->m_world.GetLocalPlayer().netid, false);
                            g_server->stateReset(g_server->m_world.GetLocalPlayer().netid);
                            shiftPressed = false;
                        }
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
            }).detach();
            return true;
        }

        else if (find_command(chat, "weather")) {
            std::string paket;
            paket =
                "\nset_default_color|`o"
                "\nadd_label_with_icon|big|`9Weather Options|left|934|"
                "\nadd_textbox|`wClick on item icon to enable visual weather|left|2480|"
                "\nadd_spacer|small|"
                "\nadd_label_with_icon_button|small|Weather Machine - Sunny|left|932|weather_0|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Beach Blast|left|830|weather_1|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Night|left|934|weather_2|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Arid|left|946|weather_3|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Rainy City|left|984|weather_5|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Harvest Moon Blast|left|1060|weather_6|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Mars Blast|left|1136|weather_7|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Spooky|left|1210|weather_8|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Growganoth|left|4150|weather_9|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Nothingness|left|1490|weather_10|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Snowy|left|1364|weather_11|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Snowy with Mad Giant|left|1364|weather_12|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Snowy with Happy Giant|left|1364|weather_13|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Undersea Blast|left|1532|weather_14|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Warp Speed|left|1750|weather_15|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Comet Dust|left|2034|weather_16|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Comet|left|2046|weather_17|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Party|left|2284|weather_18|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Pineapples|left|2744|weather_19|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Snowy Night|left|3252|weather_20|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Spring|left|3446|weather_21|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Howling Sky|left|3534|weather_22|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Heatwave (pink)|left|3694|weather_24|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Heatwave (orange)|left|3694|weather_25|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Heatwave (green)|left|3694|weather_26|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Heatwave (cyan)|left|3694|weather_27|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Heatwave (red)|left|3694|weather_28|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Stuff|left|3832|weather_29|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Pagoda|left|4242|weather_30|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Apocalypse|left|4486|weather_31|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Jungle|left|4776|weather_32|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Balloon Warz|left|4892|weather_33|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Background|left|5000|weather_34|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Autumn|left|5112|weather_35|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Valentine's|left|5654|weather_36|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - St. Paddy's Day|left|5716|weather_37|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Epoch Machine - Ice Age|left|5958|weather_38|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Epoch Machine - Volcano|left|5958|weather_39|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Epoch Machine - Floating Island|left|5958|weather_40|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Digital Rain|left|6854|weather_42|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Monochrome Blast|left|7380|weather_43|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Frozen Cliffs|left|7644|weather_44|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Surgworld Blast|left|8556|weather_45|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Bountiful Blast|left|8738|weather_46|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Meteor Shower|left|8896|weather_47|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Stargazing|left|8836|weather_48|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Startopia Final Landing|left|10214|weather_49|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Startopia Final Landing|left|6352|weather_50|noflags|0"
                "\nadd_label_with_icon_button|small|Weather Machine - Celebrity Hills|left|10286|weather_51|noflags|0|0|"
                //"\nadd_label_with_icon_button|small|Unknown|left|ITEM_ID_HERE|weather_52|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Legendary Lock|left|10410|weather_53|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Blood Dragon Lock|left|11550|weather_54|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Prince of Persia Lock|left|11586|weather_55|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Radical City Lock|left|11902|weather_58|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Plaza|left|11880|weather_59|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Nebula|left|12054|weather_60|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Protostar Landing|left|12056|weather_61|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Dark Mountains|left|12408|weather_62|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Assassin's Creed Lock|left|12654|weather_63|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Mt. Growmore|left|12844|weather_64|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Crack in Reality|left|13004|weather_65|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Nian's Mountain|left|13070|weather_66|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Rayman Lock|left|13200|weather_67|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Unknown|left|7886|weather_68|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Realm of Spirits|left|13640|weather_69|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Black Hole|left|13690|weather_70|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Rainin' Gems|left|14032|weather_71|noflags|0|0|"
                "\nadd_label_with_icon_button|small|Weather Machine - Holiday Haven|left|14094|weather_72|noflags|0|0|"

                "\nadd_spacer|small|"
                "\nadd_quick_exit|";
            "\nend_dialog|find_dialog|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
        }

        else if (find_command(chat, "switch")) {
            std::string acclist = readNamesFromFile();
            std::string paket;
            paket =
                "\nset_default_color|`o"
                "\nadd_label_with_icon|big|`9Account Switcher|left|8192|"
                "\nadd_spacer|small|"
                "\nadd_button|login_default_growid|`9" + defaultGrowId + "``|noflags|0|0|"
                "n" + acclist + ""
                "\nadd_spacer|small|"
                "\nadd_spacer|small|"
                "\nadd_button|add_account_button|`2Add `9Account``|noflags|0|0|"
                "\nadd_button|remove_account_button|`4Remove `9Account``|noflags|0|0|"
                "\nadd_spacer|small|"
                "\nadd_quick_exit|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);

            return true;
        }

        else if (find_command(chat, "proxy")) {
            std::string paket;
            paket =
                "\nstart_custom_tabs|"
                "\nadd_custom_button|myWorldsUiTab_0|image:interface/large/proxy_command.rttex;image_size:228,92;frame:1,0;width:0.15;|"
                //"\nadd_custom_button|myWorldsUiTab_1|image:interface/large/proxy_command.rttex;image_size:228,92;frame:0,1;width:0.15;|"
                //"\nadd_custom_button|myWorldsUiTab_2|image:interface/large/proxy_command.rttex;image_size:228,92;frame:0,2;width:0.15;|"
                "\nend_custom_tabs|"

                "\nadd_label_with_icon|small|`#Main Proxy Commands:``|left|5772|"
                "\nadd_spacer|small|"

                "\nadd_custom_textbox|`^/proxy|size:small|"
                "\nadd_custom_textbox|     `#>>>  `9Shows Proxy Command List Dialog|size:tiny|"
                "\nadd_spacer|small|"

                "\nadd_custom_textbox|`^/warp `w[name_world]|size:small|"
                "\nadd_custom_textbox|     `#>>>  `9Warp to a world|size:tiny|"
                "\nadd_spacer|small|"

                "\nadd_custom_textbox|`^/growscan /gscan /gs|size:small|"
                "\nadd_custom_textbox|     `#>>>  `9Show blocks and floating items in world|size:tiny|"
                "\nadd_spacer|small|"

                "\nadd_custom_textbox|`^/find `w[item_name]|size:small|"
                "\nadd_custom_textbox|     `#>>>  `9Find item and visually add it to your inventory|size:tiny|"
                "\nadd_spacer|small|"

                "\nadd_custom_textbox|`^/ping|size:small|"
                "\nadd_custom_textbox|     `#>>>  `9Shows ping latency|size:tiny|"
                "\nadd_spacer|small|"

                "\nadd_custom_textbox|`^/pf|size:small|"
                "\nadd_custom_textbox|     `#>>>  `9Enables pathfinder|size:tiny|"
                "\nadd_spacer|small|"

                "\nadd_custom_textbox|`^/relog|size:small|"
                "\nadd_custom_textbox|     `#>>>  `9Automatically leave and enter current world|size:tiny|"
                "\nadd_spacer|small|"

                "\nadd_custom_textbox|`^/switch|size:small|"
                "\nadd_custom_textbox|     `#>>>  `9Shows add account and switch account dialog|size:tiny|"
                "\nadd_spacer|small|"

                "\nadd_custom_textbox|`^/antigravity /ag|size:small|"
                "\nadd_custom_textbox|     `#>>>  `9Gives you antigravity effect|size:tiny|"
                "\nadd_spacer|small|"


                "\nadd_spacer|small|"
                "\nend_dialog|proxy_tab||Back|"
                "\nadd_quick_exit|";

            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);

            return true;
        }



        //FOR TESTING ONLY

        else if (find_command(chat, "inv")) {
            auto inventory = g_server->local_player.inventory;

            for (const auto& entry : inventory.items) {
                std::cout << "Item ID: " << entry.second.id
                    << ", Count: " << static_cast<int>(entry.second.count) << std::endl;
            }
            return true;
        }

        else if (find_command(chat, "ready")) {
            for (auto& tile : g_server->m_world.GetTileMap().tiles) {
                if (tile.foreground == 3) {
                    //cout << "\n" + to_string(tile.tileExtra.seed.seedTime);
                    //cout << "\n" + to_string(tile.tileExtra.seed.plantTime);
                    cout << "\n" + to_string(tile.tileExtra.seed.readyTime);
                    cout << "\n" + to_string(tile.tileExtra.seed.fruit_count) + "\n";
                }
            }
            return true;
        }
        else if (find_command(chat, "door")) {
            for (auto& tile : g_server->m_world.GetTileMap().tiles) {
                if (tile.foreground == 12 || tile.background == 12) {
                    //cout << "\n" + to_string(tile.tileExtra.seed.seedTime);
                    //cout << "\n" + to_string(tile.tileExtra.seed.plantTime);
                    cout << "\nDOOR";
                    cout << "\n" + tile.tileExtra.door.label + "\n";
                }
            }
            return true;
        }

        else if (find_command(chat, "inv")) {
            auto& inventory = g_server->local_player.inventory.items;
            for (auto it = inventory.begin(); it != inventory.end(); ++it) {
                cout << "\nItem ID: " << to_string(it->second.id) << "(" << to_string(it->second.count) << "x)\n";
            }
            return true;
        }

        else if (find_command(chat, "glow")) {
            for (auto& tile : g_server->m_world.GetTileMap().tiles) {
                //green 16402

                if (tile.foreground == 2010 || tile.background == 2010) {
                    cout << "\nGlowy Flag: " + to_string(tile.flag) + "\n";
                    cout << "\nGlowy X: " + to_string(tile.position.X) + "\n";
                    cout << "\nGlowy Y: " + to_string(tile.position.Y) + "\n";
                }
            }
            return true;
        }

        else if (find_command(chat, "asd")) {
            for (auto& tile : g_server->m_world.GetTileMap().tiles) {

                if (tile.foreground == 226 || tile.background == 226) {
                    cout << "\nSignal Jammer Flag: " + to_string(tile.flag) + "\n";
                    cout << "\nSignal Jammer X: " + to_string(tile.position.X) + "\n";
                    cout << "\nSignal Jammer Y: " + to_string(tile.position.Y) + "\n";
                }

                if (tile.foreground == 5032 || tile.background == 5032) {
                    cout << "\nFissure Flag: " + to_string(tile.flag) + "\n";
                }

                if (tile.foreground == 5036 || tile.background == 5036) {
                    cout << "\nHidden Door Flag: " + to_string(tile.flag) + "\n";
                }
            }
            return true;
        }

        else if (find_command(chat, "vtrash")) {
                    string visual_item_list = "";
                    for (const auto& value : visual_item_id) {
                        cout << "Visual Item ID: " + to_string(value) + "\n";
                        visual_item_list += "\nadd_label_with_icon_button|small|" + items[value].name + "|left|" + to_string(value) + "|remove_visual_btn_" + to_string(value) + "|noflags|0|0|";
                    }

                    std::string paket;
                    paket =
                        "\nadd_label_with_icon|big|`9Remove Visual Items|left|1784|"
                        "\nadd_textbox|`wClick on item icon to remove it from your inventory|left|2480|"
                        "\nadd_spacer|small|"\
                        "\n" + visual_item_list + ""
                        "\nadd_quick_exit|"
                        "\nend_dialog|find_dialog|Cancel|Okay|";
                    variantlist_t liste{ "OnDialogRequest" };
                    liste[1] = paket;
                    g_server->send(true, liste);

                    return true;
        }

        else if (find_command(chat, "test")) {
                    cout << "\n" + to_string(g_server->m_world.GetTileMap().worldSize.X);
                    cout << "\n" + to_string(g_server->m_world.GetTileMap().worldSize.Y) + "\n";
                    return true;
            }

        else if (find_command(chat, "try")) {

                    gt::send_log(g_server->m_world.GetLocalPlayer().name);
                    
                    return true;
            }
        //88 //30 ac area //48 green effect good for load visual particleeffect

        else if (find_command(chat, "cmod")) {
            g_server->send(false, "action|dialog_return\ndialog_name|friends\nbuttonClicked|56766032", 2);
            g_server->send(false, "action|dialog_return\ndialog_name|friends_edit\nfriendID|56766032|\nbuttonClicked|msg", 2);
            g_server->send(false, "action|dialog_return\ndialog_name|friends_message\nfriendID|56766032|\nbuttonClicked|send\ntext|dawdwa", 2);
            return true;
        }

        return false;
    }

    if (packet.find("game_version|") != -1) {
        rtvar var = rtvar::parse(packet);


/*
        if (g_server->m_server == "213.179.209.168") {
            rtvar var1;
            using namespace httplib;
            Headers Header;
            Header.insert(std::make_pair("User-Agent", "UbiServices_SDK_2019.Release.27_PC64_unicode_static")); //2019 to 2022
            //Header.insert(std::make_pair("User-Agent", "UbiServices_SDK_2022.Release.9_PC64_ansi_static"));
            Header.insert(std::make_pair("Host", "www.growtopia1.com"));
            Client cli("https://104.125.3.135");
            cli.set_default_headers(Header);
            cli.enable_server_certificate_verification(false);
            cli.set_connection_timeout(2, 0);
            auto res = cli.Post("/growtopia/server_data.php");
            if (res.error() == Error::Success) {
                var1 = rtvar::parse({ res->body });
            }
            else
            {
                Headers Header;
                Header.insert(std::make_pair("User-Agent", "UbiServices_SDK_2019.Release.27_PC64_unicode_static"));
                //Header.insert(std::make_pair("User-Agent", "UbiServices_SDK_2022.Release.9_PC64_ansi_static"));
                Header.insert(std::make_pair("Host", "www.growtopia1.com"));
                Client cli("https://104.125.3.135");
                cli.set_default_headers(Header);
                cli.enable_server_certificate_verification(false);
                cli.set_connection_timeout(2, 0);
                auto res = cli.Post("/growtopia/server_data.php");
                if (res.error() == Error::Success)
                    var1 = rtvar::parse({ res->body });
            }
            g_server->meta = (var1.find("meta") ? var1.get("meta") : (g_server->meta = var1.get("meta")));
            print::set_info("[SERVER]: ", std::string("Server Meta: ").append(g_server->meta + "\n").c_str(), LightBlue);
            if (g_server->meta.empty()) {
                print::set_info("[PROXY]: ", " Failed retrieving meta from growtopia server\n", Red);
                print::set_info("[PORYX]: ", " If you are using VPN try to change ip\n", Red);
            }

        }
*/


        defaultGrowId = var.get("tankIDName");

        if (switchAccount) {
            var.set("tankIDName", switchGrowid);
            var.set("tankIDPass", switchGrowidPass);
        }

        auto mac = utils::generate_mac();
        var.set("mac", mac);
        auto hash_str = mac + "RT";
        auto hash2 = utils::hash((uint8_t*)hash_str.c_str(), hash_str.length());
        var.set("wk", utils::generate_rid());
        auto rid = utils::generate_rid();
        var.set("rid", rid);
        var.set("hash", std::to_string(utils::random(INT_MIN, INT_MAX)));
        var.set("hash2", std::to_string(hash2));
        var.set("klv", proton::generate_klv(var.get_int("protocol"),var.get("game_version"), rid));
        var.set("meta", g_server->meta);
        packet = var.serialize();
        packet += "requestedName|\n";
        packet += "tankIDPass|\n";
        gt::in_game = false;
        gt::send_log("`2Spoofing Login Info...");
        g_server->send(false, packet);
        return true;
    }

    return false;
}

bool events::out::gamemessage(std::string packet) {
    if (debug)
        PRINTS("\nGame message: %s\n", packet.c_str());
    if (packet == "action|quit") {
        g_server->quit();
        return true;
    }
    return false;
}

bool events::out::state(GameUpdatePacket* packet, gameupdatepacket_t* packet2) {
    if (!g_server->m_world.connected)
        return false;

    if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) && pf) {
        if (GetKeyState(VK_LBUTTON) & 0x8000) {
            g_server->MoveXY(packet2->m_state1, packet2->m_state2);
            return true;
        }
    }
    if (startAutoFish) {
        if (packet->flags == 32) playerFaceFlag = 2592;
        else if (packet->flags == 48) playerFaceFlag = 2608;

        cout << "\nPACKET->FLAGS: " + to_string(playerFaceFlag) + "\n";
    }

    if (showLatencyPing) {
        variantlist_t va{ "OnNameChanged" };
        va[1] = "`^[" + to_string(g_server->GetPing()) + "] ``" + g_server->m_world.GetLocalPlayer().name;   //visualName;
        g_server->send(true, va, g_server->m_world.GetLocalPlayer().netid, -1);
        return true;
    }


    if (punch_tile_gems) {
        if (packet->flags == 2592 || packet->flags == 2608) {
            totalGemsInSpecificTiles(packet2->m_state1, packet2->m_state2);
        }
    }

    g_server->m_world.GetLocalPlayer().pos = vector2_t{ packet->vec_x, packet->vec_y };
    //if (debug) packet->debug_print("wkwkw");
    //PRINTS("Local Pos: %.0f %.0f\n", packet->vec_x / 32, packet->vec_y / 32);

    if (gt::ghost)
        return true;
    return false;
}

vector<string> split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}


bool events::in::variantlist(gameupdatepacket_t* packet) {
    variantlist_t varlist{};
    auto extended = utils::get_extended(packet);
    extended += 4;
    varlist.serialize_from_mem(extended);
    if (debug) {
        PRINTS("\nVarlist: %s\n", varlist.print().c_str());
    }
    auto func = varlist[0].get_string();
    //probably subject to change, so not including in switch statement.
    if (func.find("OnSuperMain") != -1) { //StartAcceptLogon
        gt::in_game = true;
    }
    auto func2 = varlist[1].get_string();
    if (func.find("OnPlayPositioned") != -1) {
        if (startAutoFish) {
            if (func2.find("audio/splash.wav") != -1) {
                if (g_server->local_player.inventory.getItemCount(baitItemId) == 0) {
                    if (playerFaceFlag == 2592) {
                        auto_fish(baitItemId, (g_server->m_world.GetLocalPlayer().pos.m_x / 32) + 1, (g_server->m_world.GetLocalPlayer().pos.m_y / 32) + 1);
                    }
                    else if (playerFaceFlag == 2608) {
                        auto_fish(baitItemId, (g_server->m_world.GetLocalPlayer().pos.m_x / 32) - 1, (g_server->m_world.GetLocalPlayer().pos.m_y / 32) + 1);
                    }
                    startAutoFish = false;
                    gt::send_log("`4Auto Fish Disabled!`9Not Enough Bait in Backpack");
                    variantlist_t notif{ "OnTextOverlay" };
                    notif[1] = "`9Auto Fish `4Disabled! `9Not Enough Bait";
                    g_server->send(true, notif, -1, -1);
                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    if (playerFaceFlag == 2592) {
                        auto_fish(baitItemId, (g_server->m_world.GetLocalPlayer().pos.m_x / 32) + 1, (g_server->m_world.GetLocalPlayer().pos.m_y / 32) + 1);
                    }
                    else if (playerFaceFlag == 2608) {
                        auto_fish(baitItemId, (g_server->m_world.GetLocalPlayer().pos.m_x / 32) - 1, (g_server->m_world.GetLocalPlayer().pos.m_y / 32) + 1);
                    }
                }
            }
        }
    }

/*
    if (func.find("OnFlagMay2019") != -1) {
        gt::send_log("ghost");
        gt::send_log(to_string(varlist[2].get_int32()));
    }
*/

    switch (hs::hash32(func.c_str())) {
    case fnv32("OnSetRoleSkinsAndIcons"): {
        skinas = varlist[5].get_string();
    } break;
    case fnv32("OnRequestWorldSelectMenu"): {
/*
        old_world = g_server->m_world.GetName();
        auto& world = g_server->m_world;
        world.GetPlayers().clear();
        world.GetObjectMap().objects.clear();
        world.GetLocalPlayer() = {};
        world.connected = false;
        world.GetName() = "EXIT";
*/
        g_server->m_world.connected = false;
        g_server->m_world.ResetData();
    } break;
    case fnv32("OnSendToServer"): g_server->redirect_server(varlist); return true;
    case fnv32("OnConsoleMessage"): {
        auto wry = varlist[1].get_string();

        if (son && wry.find("`6>>`4Spam detected! ``Please wait a bit before typing anything else.  Please note, any form of bot/macro/auto-paste will get all your accounts banned, so don't do it!") != -1) {
            variantlist_t varlist2{ "OnAddNotification" };
            varlist2[1] = 0;
            varlist2[2] = "`2Spam Stopped because spam detected!";
            varlist2[3] = 0;
            varlist2[4] = 0;
            g_server->send(true, varlist2);
            son = false;
            go_mode = false;

        }



        varlist[1] = "`4[peanut]`` " + varlist[1].get_string();
        g_server->send(true, varlist);
        return true;
    }break;
    case fnv32("OnTalkBubble"): {
        auto wry = varlist[2].get_string();

        auto str = varlist[2].get_string();
        auto netid = varlist[1].get_int32();
        auto& bruh = g_server->m_world.GetLocalPlayer();

        g_server->send(true, varlist);
        return true;
    } break;
    case fnv32("OnDialogRequest"): {
        auto content = varlist[1].get_string();
        if (dropwl == true) {
            if (content.find("Drop") != -1) {
                dropwl = false;
                return true;
            }
        }
        if (unaccessing == true) {
            if (content.find("unaccess") != -1) {
                unaccessing = false;
                return true;
            }
        }
        if (cdrop == true) {
            if (content.find("Drop") != -1) {
                cdrop = false;
                return true;
            }
        }
        else if (dropdl == true) {
            if (content.find("Drop") != -1) {
                dropdl = false;
                return true;
            }
        }
        else if (dropbgl == true) {
            if (content.find("Drop") != -1) {
                dropbgl = false;
                return true;
            }
        }

        if (wrenchModeOnOff) {
            if (content.find("set_default_color|`o") != -1) {
                if (content.find("embed_data|netID") != -1) {
                    return true;
                }
            }
        }

        if (content.find("add_label_with_icon|big|`wThe Growtopia Gazette``|left|5016|") != -1)
        {
            std::string paket = "add_label_with_icon|big|`4Yow Proxy Gazette``|left|828|"
                "\nadd_spacer|small|"
                "\nadd_label_with_icon|big|`2All Proxy Commands:|left|5956|"
                "\nadd_label_with_icon|small|" + first + "/warp [world name] " + second + "(warps to world of your choosen just like a supporter)|left|482|"
                "\nadd_quick_exit|"
                "\nset_survey_enabled|1"
                "\nend_dialog|gazette||OK|";
            varlist[1] = paket;
            g_server->send(true, varlist);
            return true;
        }
    }break;
    case fnv32("OnGuildDataChanged"): {
        auto varlist1 = varlist[1].get_int32();
        auto varlist3 = varlist[3].get_int32();

        if (varlist1 == 15519) {
            //mod = true;

        }
        else if (varlist1 == 45250 && varlist3 == 276) {
            //
        }
        else if (varlist3 == 5956 || varlist3 == 276) {

        }

    }break;
    case fnv32("OnRemove"): {
        auto text = varlist.get(1).get_string();
        if (text.find("netID|") == 0) {
            auto netid = atoi(text.substr(6).c_str());

            if (netid == g_server->m_world.GetLocalPlayer().netid) {
                g_server->m_world.GetLocalPlayer() = {};
                g_server->m_world.GetObjectMap().objects.clear();


            }
            auto& players = g_server->m_world.GetPlayers();
            for (size_t i = 0; i < players.size(); i++) {
                auto& player = players[i];
                if (player.netid == netid) {
                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                    break;
                }
            }
        }
    } break;
    case fnv32("OnSpawn"): {

        std::string meme = varlist.get(1).get_string();
        rtvar var = rtvar::parse(meme);
        auto name = var.find("name");
        auto netid = var.find("netID");
        auto onlineid = var.find("onlineID");
        if (name && netid && onlineid) {
            player ply{};
            ply.mod = false;
            ply.invis = false;
            ply.name = name->m_value;
            ply.country = var.get("country");
            name->m_values[0] += "";

            auto pos = var.find("posXY");
            if (pos && pos->m_values.size() >= 2) {
                auto x = atoi(pos->m_values[0].c_str());
                auto y = atoi(pos->m_values[1].c_str());
                ply.pos = vector2_t{ float(x), float(y) };
            }
            ply.userid = var.get_int("userID");
            ply.netid = var.get_int("netID");
            if (meme.find("type|local") != -1) {
                //set mod state to 1 (allows infinite zooming, this doesnt ban cuz its only the zoom not the actual long punch)
                var.find("mstate")->m_values[0] = "1";
                localPlayerName = var.get("name");
                g_server->m_world.GetLocalPlayer() = ply;
                if (antigravity) anti_gravity();
            }
            g_server->m_world.GetPlayers().push_back(ply);
            auto str = var.serialize();
            utils::replace(str, "onlineID", "onlineID|");
            varlist[1] = str;
            //PRINTC("new: %s\n", varlist.print().c_str());
            g_server->send(true, varlist, -1, -1);
            return true;
        }
    } break;
    case fnv32("OnSetPos"): {
        if (gt::noclip)
            return true;
        return false;
    }
    case fnv32("OnNameChanged"): std::thread(itsmod, packet->m_player_flags, varlist[1].get_string()).detach(); return false;
    }
    return false;
}

bool events::in::generictext(std::string packet) {
    if (iswear) send_vset();
    return false;
}

bool events::in::gamemessage(std::string packet) {
    if (gt::resolving_uid2) {
        if (packet.find("PERSON IGNORED") != -1) {
            g_server->send(false, "action|dialog_return\ndialog_name|friends_guilds\nbuttonClicked|showfriend");
            g_server->send(false, "action|dialog_return\ndialog_name|friends\nbuttonClicked|friend_all");
        }
        else if (packet.find("Nobody is currently online with the name") != -1) {
            gt::resolving_uid2 = false;
            gt::send_log("Target is offline, cant find uid.");
        }
        else if (packet.find("Clever perhaps") != -1) {
            gt::resolving_uid2 = false;
            gt::send_log("Target is a moderator, can't ignore them.");
        }
    }
    return false;
}

bool events::in::state(gameupdatepacket_t* packet) {
    if (!g_server->m_world.connected)
        return false;
    if (packet->m_player_flags == -1)
        return false;

    auto& players = g_server->m_world.GetPlayers();

    for (auto& player : players) {
        if (player.netid == packet->m_player_flags) {
            player.pos = vector2_t{ packet->m_vec_x, packet->m_vec_y };
            break;
        }
    }

    return false;
}

bool events::in::tracking(std::string packet) {
    //PRINTC("Tracking packet: %s\n", packet.c_str());
    if (packet.find("eventName|102_PLAYER.AUTHENTICATION") != -1)
    {
        string wlbalance = packet.substr(packet.find("Worldlock_balance|") + 18, packet.length() - packet.find("Worldlock_balance|") - 1);

        if (wlbalance.find("PLAYER.") != -1)
        {
            gt::send_log("`9World Lock Balance: `#0");
        }
        else
        {
            gt::send_log("`9World Lock Balance: `#" + wlbalance);

        }
        if (packet.find("Authenticated|1") != -1)
        {
            gt::send_log("`9Player Authentication `2Successfuly.");
        }
        else
        {
            gt::send_log("`9Player Authentication `4Failed.");
        }

    }
    if (packet.find("eventName|100_MOBILE.START") != -1)
    {
        gems = packet.substr(packet.find("Gems_balance|") + 13, packet.length() - packet.find("Gems_balance|") - 1);
        level = packet.substr(packet.find("Level|") + 6, packet.length() - packet.find("Level|") - 1);
        uid = packet.substr(packet.find("GrowId|") + 7, packet.length() - packet.find("GrowId|") - 1);
        gt::send_log("`9Gems Balance: `#" + gems);
        gt::send_log("`9Account Level: `#" + level);
        gt::send_log("`9Your Current UID: `#" + uid);
    }
    if (packet.find("eventName|300_WORLD_VISIT") != -1)
    {
        if (packet.find("Locked|0") != -1)
        {
            gt::send_log("`4This world is not locked by a world lock.");
        }
        else
        {
            gt::send_log("`2This world is locked by a world lock.");

            if (packet.find("World_owner|") != -1)
            {
                string uidd = packet.substr(packet.find("World_owner|") + 12, packet.length() - packet.find("World_owner|") - 1);
                gt::send_log("`9World Owner UID: `#" + uidd);

            }
        }
    }

    return true;
}