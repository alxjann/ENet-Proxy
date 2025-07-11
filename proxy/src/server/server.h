#pragma once
#include <string>
#include "../../vendor/proton/variant.hpp"
#include "../../vendor/enet/include/enet.h"
#include "../world/World.h"
#include <queue>
#include "../player/struct.h"
#include <thread>
#include <mutex>
using namespace std;
class server {
private:
    ENetHost* m_proxy_server;
    ENetHost* m_real_server;
    ENetPeer* m_server_peer;
    ENetPeer* m_gt_peer;
    
    void handle_outgoing();
    void handle_incoming();
    bool connect();
public:
    ENetPeer* m_peer;
    void disconnectsr(bool reset);

    int m_user = 0;
    int m_token = 0;
    int gtmap[60][100];
    LocalPlayer local_player;
    std::string m_server = "213.179.209.168";
    int m_port = 17022; //17022
    std::string serverz = m_server;
    int portz = m_port;
    int m_proxyport = 17191;//17191;
    std::string ipserver = "127.0.0.1";
    std::string create = "0.0.0.0";
    std::string meta = "NULL";
    World m_world;

    bool start();
    void quit();
    bool setup_client();
    void redirect_server(variantlist_t& varlist);
    void send(bool client, int32_t type, uint8_t* data, int32_t len);
    void send(bool client, variantlist_t& list, int32_t netid = -1, int32_t delay = 0);
    void send(bool client, std::string packet, int32_t type = 2);
    void longpunch(int netid2, bool enable);
    void SendPacketRaw(bool client, int a1, void* packetData, size_t packetDataSize, void* a4, int packetFlag);
    void SendPacketRaw2(bool client, int a1, void* packetData, size_t packetDataSize, void* a4, int packetFlag);
    static void addBlock(int a, int b, int c);
    static void placeBlock(bool client, int a, int b, int c);
    static void breakBlock(bool client, int x, int y);
    static void enterDoor(bool client, int x, int y);
    static void wearItem(int itemid);

    void sendStateTest(int netid2, int statenum);

    int GetPing();
    WorldTile* GetTile(int x, int y);
    void sendState(int netid2);
    static void speedy(int netid, int XSpeed);
    void MoveXY(int x, int y);
    void NoclipState(int netid);
    void NoclipState2(int netid);
    void stateReset(int netid2);
    bool sendEnetPacket(ENetPacket* packet, bool client);
    void poll();
    void lockThread();
    void unlockThread();
    std::mutex mutex;
    std::atomic<bool> mutexStatus;
    uint32_t threadID;
    std::condition_variable cv;
};
extern server* g_server;

