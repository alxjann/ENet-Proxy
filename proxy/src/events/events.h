#pragma once
#include "../../vendor/enet/include/enet.h"
#include <string>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <thread>
#include "../../vendor/proton/vector.hpp"
#include "../../vendor/proton/variant.hpp"
#include <mutex>
#include <queue>
#include "../world/World.h"

//return value: true - dont send original packet, false - send original packet
namespace events {

    namespace out {
        bool variantlist(gameupdatepacket_t* packet);
        bool pingreply(gameupdatepacket_t* packet);
        bool generictext(std::string packet);
        bool gamemessage(std::string packet);
        bool state(GameUpdatePacket* packet, gameupdatepacket_t* packet2);
        int get_punch_id(const int id_);

    }; // namespace out
    namespace in {
        bool variantlist(gameupdatepacket_t* packet);
        bool generictext(std::string packet);
        bool gamemessage(std::string packet);
        //bool sendmapdata(gameupdatepacket_t* packet, int packetLength, enet_uint8* packetData);
        bool state(gameupdatepacket_t* packet);
        bool tracking(std::string packet);
    }; // namespace in
};     // namespace events

void totalGemsInSpecificTiles(int targetPosX, int targetPosY);
void autoc();

bool iswear = false;
float hair, shirt, pants = 0.f; //cloth_hair & cloth_shirt & cloth_pants
float shoe, face, hand = 0.f; //cloth_feet & cloth_face & cloth_hand
float back, mask, neck = 0.f;//cloth_back cloth_mask cloth_necklace
float ances = 0; //cloth_ances
float hat = 0.f;
bool bruhmoment = false;
std::vector<std::thread> sel;
bool rmode = false;
std::vector<int> visual_item_id;
bool pathfinder = false;

bool switchAccount = false;
string switchGrowid = "";
string switchGrowidPass = "";
string defaultGrowId = "";

int weatherVar1;
bool pf = false;

bool shiftPressed = false;

bool showAutoCollectEffect = false;
bool autoCollect = false;
bool hasPf = false;

//Gems Option
bool collected_gems = false;
bool instant_gems = false;
bool punch_tile_gems = false;

string visualName = "";//g_server->m_world.local.name;

bool showLatencyPing = false;

bool showGemsInstantDrop_running = false;
bool showGemsCollected_running = false;

string wrenchMode = "None";
bool wrenchModeOnOff = false;
bool wrenchRightKick = false;
string wrenchPullFrame = "staticBlueFrame";
string wrenchKickFrame = "staticBlueFrame";
string wrenchBanFrame = "staticBlueFrame";
string autoPullFrame = "staticBlueFrame";
string autoBanFrame = "staticBlueFrame";
string autoPullName = "None";

int baitItemId = 0;
bool startAutoFish = false;

bool debug = false;

int oldGemCount;
//bool gemsInstantDrop = false;

bool gemsDrop = false;
int instantDropX;
int instantDropY;

bool showCollectedGems = false;
int totalCollectedGems = 0;

bool stoppf = false;

bool iswearOnBefore;
int punchEffectBeforePf;

bool auto_farm = false;

string localPlayerName = "";
int visual_trash_id = 0;
float m_x = 0.0;
float m_y = 0.0;

uint32_t item_id;
int wrenchx = 0;
int wrenchy = 0;
bool scan = false;

bool doublejump = false;
bool noface = false;
bool onering = false;
bool devmode = false;

int w_id = 0;
int yuzde = 10;
bool ssup = false;
int farmhit = 4;
float speed_x = 250.0f;
int skincolor = 1685231359;
uint8_t x_punch = 128;
int authfail = -1;
bool pfchecker = false;

float speed_y = 1000.0f;
int reme_delay = 250;
int punch_delay = 100;
int bruh;
int bruh2;
int bruh3;
int bruh4;
int fcount;
int afid = 0;
int countbet;
int ruletsayi = 0;
bool go_mode = false;
bool enabled_color = false;
bool free_version = false;
int delay = 4000;
std::vector<std::string> adamlar = {};
std::string wldrop = "";
std::string wldrop2 = "";
std::string bc = "0";
std::string lc = "0";
std::string lidb = "0";
std::string first = "`2";
std::string second = "`9";
std::string value = "";
std::string mode = "`5Pull";
std::string skinas = "";
std::string dcount = "0";
std::string aspam = "Hello";
std::string growid = "";
std::string password = "";
std::string wlbalance = "";
std::string level = "";
std::string uid = "";
std::string gems = "";
std::string kick1x = "";
std::string kick2x = "";
std::string kick3x = "";
std::string kick4x = "";
std::string kick1y = "";
std::string backworld = "";
std::string pullvisus = "0";
std::string swx = "0";
std::string swxs = "0";
std::string c_text = "";
std::string old_world = "EXIT";
std::string banvisus = "0";
std::string pname = "0";
std::string bname = "0";
std::string vardaspull = "";
std::string vardasban = "";

bool qeme = false;

bool notax_mode = true;
int wl_count = 0;
int dl_count = 0;
int bgl_count = 0;
bool blink = false;
bool cdrop = false;
bool pullbynames = false;
bool banbynames = false;
bool nezinau = false;
bool son = false;
bool used_sponge = false;
bool is_spam = false;
bool enter_pull = false;
bool enter_ban = false;
bool exit_world = false;
bool unc_world = false;
bool ban_all = false;
bool wldropfast = false;
bool mod_detect = false;
bool shouldresetfastwl = false;
bool auto_surg = false;
bool wrench = false;
bool fasttrash = false;
bool fastdrop = false;
bool antigravity = false;
int total_bet = 0;
bool game_started = false;
bool pathfinding = false;
bool noclip = false;
int punch_effect = 8421376;
bool autoacc = false;
bool is_blink = false;
bool visualspin = false;
bool dropwl = false;
int pathamount = 0;
bool fastvend = false;
string expectcount = "0";
string lastshop = "";
bool dropdl = false;
bool shopmode = false;
bool sellectshopmode = false;
bool farmauto = false;
int farmid = 2;
bool dropbgl = false;
bool done = true;
bool cbet1 = false;
bool cbet2 = false;
bool modas = false;
std::string hotkeys1 = "";
std::string hotkeys2 = "";
std::string hotkeys3 = "";
std::string hotkeys4 = "";
std::string hotkeys5 = "";
std::string hotkeys6 = "";
std::string hotkeys7 = "";
std::string hotkeys8 = "";
std::string hotkeys9 = "";
std::string hotkeys10 = "";
std::string hotkeys11 = "";
std::string hotkeys12 = "";

vector2_t punchpos;
vector2_t secondpos;
vector2_t pos1;
vector2_t pos2;

vector2_t autohost_pos1;
vector2_t autohost_pos2;
vector2_t autohost_pos3;
vector2_t autohost_pos4;

bool blockfirst = false;