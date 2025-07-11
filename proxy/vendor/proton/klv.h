#pragma once
//#define _CRT_SECURE_NO_WARNINGS
#include <array>
#include <string>
#include "./hash2.h"

#include <algorithm>
#include <openssl/evp.h>

#include <spdlog/fmt/bin_to_hex.h>

namespace proton {
    static std::string generate_klv(const int protocol, const std::string& version, const std::string& rid) {
        constexpr std::array salts = {
            "e9fc40ec08f9ea6393f59c65e37f750aacddf68490c4f92d0d2523a5bc02ea63",
            "c85df9056ee603b849a93e1ebab5dd5f66e1fb8b2f4a8caef8d13b9f9e013fa4",
            "3ca373dffbf463bb337e0fd768a2f395b8e417475438916506c721551f32038d",
            "73eff5914c61a20a71ada81a6fc7780700fb1c0285659b4899bc172a24c14fc1"
        };

        static std::array constant_values = {
            hash123::sha256(hash123::md5(hash123::sha256(std::to_string(protocol)))),
            hash123::sha256(hash123::sha256(version)),
            hash123::sha256(hash123::sha256(std::to_string(protocol)) + salts[3])
        };

        return hash123::sha256(constant_values[0]
            + salts[0]
            + constant_values[1]
            + salts[1]
            + hash123::sha256(hash123::md5(hash123::sha256(rid)))
            + salts[2]
            + constant_values[2]
        );
    }
}