#ifndef SERVER_H
#define SERVER_H
#include <map>
#include <string>

#include "CSockets.h"

class Server
{
public:
	Server();
	virtual ~Server();

	void Start(unsigned short port);
	void HandlePacket(const char* packet, const sockaddr_in& from);
	void HandleConnectionRequest(const CSockets::ConnectionRequestPacket* request, const sockaddr_in& from);

    void HandlePlayerPosition(const CSockets::PlayerStatePacket* position, const sockaddr_in& from);
    void HandlePlayerAction(const CSockets::PlayerActionPacket* action, const sockaddr_in& from);
    void HandlePlayerDisconnect(unsigned int uid, const sockaddr_in& from);
    void HandleProjectilePosition(const CSockets::ProjectilePositionPacket* position, const sockaddr_in& from);

    void BroadcastGameState();
    void BroadCastProjectilesStates();

private:

    struct PlayerInfo 
    {
        unsigned int uid;
        std::string playerName;
        int hp;
        float x, y;
    };

    struct ProjInfo
    {
        unsigned int uid;
        float x, y;
    };

    static DWORD WINAPI ReceiveThread(LPVOID param)
    {
        Server* server = static_cast<Server*>(param);
        char buffer[1024];
        sockaddr_in from;

        while (true)
        {
            if (server->mSocket.ReceiveFrom(buffer, sizeof(buffer), from))
            {
                EnterCriticalSection(&server->mCriticalSection);
                server->HandlePacket(buffer, from);
                LeaveCriticalSection(&server->mCriticalSection);
            }
        }

        return 0;
    }

private:
	
	std::map<unsigned int, sockaddr_in> mClients;
    std::map<unsigned int, PlayerInfo> mPlayers;
    std::map<unsigned int, ProjInfo> mProjectiles;
	unsigned int mNextUid;

public:
    CSockets mSocket;
    CRITICAL_SECTION mCriticalSection;
};

#endif // !SERVER_H

