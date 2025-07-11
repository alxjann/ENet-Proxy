#pragma once
#include <string>
#include "../packet/packet.h"

namespace gt {
    extern std::string version;
    extern std::string flag;
    extern std::string macaddr;
    extern int block_id;
    extern int max_dropped_block;
    extern bool showxy;
    extern int ac_range;
    extern bool resolving_uid2;
    extern bool connecting;
    extern bool aapbypass;
    extern int path_amount;
    extern bool resolving_uid;
    extern bool resolving_uid2;
    extern bool in_game;
    extern bool ghost;
    extern bool noclip;
    extern bool noclip2;
    extern bool doublejump;
    extern bool autocollect;


    extern bool ruzgar;
    void send_log(std::string text);
    void findpath(int x, int y);
}
