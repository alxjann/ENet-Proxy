#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include "../../vendor/enet/include/enet.h"
#include "../packet/packet.h"
#include "../../vendor/proton/variant.hpp"
#include <mutex>
#include <__msvc_chrono.hpp>
using namespace std::chrono;

#define PRINTS(msg, ...) printf("\033[1;32m[SERVER]:\033[0m \033[1;34m" msg "\033[0m", ##__VA_ARGS__);
#define PRINTC(msg, ...) printf("\033[1;32m[CLIENT]:\033[0m \033[1;34m" msg "\033[0m", ##__VA_ARGS__);


#define MALLOC(type, ...) (type*)(malloc(sizeof(type) __VA_ARGS__))
#define get_packet_type(packet) (packet->dataLength > 3 ? *packet->data : 0)
#define DO_ONCE            \
    ([]() {                \
        static char o = 0; \
        return !o && ++o;  \
    }())
#ifdef _WIN32
#define INLINE __forceinline
#else //for gcc/clang
#define INLINE inline
#endif

namespace utils {
    char* get_text(ENetPacket* packet);
    gameupdatepacket_t* get_struct(ENetPacket* packet);
    GameUpdatePacket* get_structGUP(ENetPacket* packet);

    int random(int min, int max);
    std::string generate_rid();
    std::string genhash();
    uint32_t hash(uint8_t* str, uint32_t len);
    std::string get_type_string(uint8_t type);
    std::string generate_mac(const std::string& prefix = "02");
    std::string hex_str(unsigned char data);
    std::string random(uint32_t length);
    INLINE uint8_t* get_extended(gameupdatepacket_t* packet) {
        return reinterpret_cast<uint8_t*>(&packet->m_data_size);
    }
    INLINE uint8_t* GetExtendedData(gameupdatepacket_t* packet) {
        return reinterpret_cast<uint8_t*>(&packet->m_data);
    }
/*
    inline std::uint8_t* GetExtendedData(gameupdatepacket_t* game_update_packet) {
        if (!(game_update_packet->m_packet_flags & 1 << 3))
            return nullptr;

        struct ExtendedPacket {
            std::uint8_t pad[sizeof(gameupdatepacket_t)];
            std::uint32_t data;
        };

        return reinterpret_cast<std::uint8_t*>(&reinterpret_cast<ExtendedPacket*>(game_update_packet)->data);
    }
*/

    bool replace(std::string& str, const std::string& from, const std::string& to);
    bool is_number(const std::string& s);

    bool runAtInterval(time_point<system_clock>& timer, double interval) {
        auto now = system_clock::now();
        auto elapsed = now - timer;

        if (elapsed >= seconds(static_cast<long long>(interval))) {
            timer = now;
            return true;
        }

        return false;
    }

} // namespace utils
