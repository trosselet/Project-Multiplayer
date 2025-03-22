#pragma once
#include "Thread.h"

template<typename T>
class CVector2;

class CSockets
{

    /*class ThreadInternal : public Thread
    {
        SOCKET* m_pSocket;
        virtual void OnEnter()
        {
            int fromSize = sizeof(from);
            int received = recvfrom(mSocket, static_cast<char*>(buffer), bufferSize, 0, (sockaddr*)&from, &fromSize);
            return received > 0;
        }
    };*/

private:
	SOCKET mSocket;
	sockaddr_in mAddress;

public:
	CSockets();
	virtual ~CSockets();

	bool Init(unsigned int port);
	bool SendTo(const char* ip, unsigned int port, const void* data, int dataSize);
	bool ReceiveFrom(void* buffer, int bufferSize, sockaddr_in& from);

public:
    enum PacketType
    {
        CONNECTION_REQUEST,
        CONNECTION_ACCEPTED,
        PLAYER_POSITION,
        PLAYER_ACTION,
        PLAYER_DISCONNECT,
        GAME_STATE,
        PROJECTILE_POSITION,
        PROJECTILE_STATE,
    };

    struct PacketHeader
    {
        PacketType type;
        unsigned int uid;
    };

    struct ConnectionRequestPacket
    {
        PacketHeader header;
        char playerName[32];
    };

    struct ConnectionAcceptedPacket
    {
        PacketHeader header;
        unsigned int autoUid;
    };

    struct PlayerStatePacket
    {
        PacketHeader header;
        float x;
        float y;
        int pv;
    };

    struct ProjectilePositionPacket
    {
        PacketHeader header;
        float x;
        float y;
    };

    struct PlayerActionPacket
    {
        PacketHeader header;
        bool isShooting;
    };

    struct GameStatePacket
    {
        PacketHeader header;
        unsigned int playersScores[16] = { 0 };
        unsigned int hpBar[16] = { 0 };
        float x[16] = { 0 };
        float y[16] = { 0 };
    };

    struct ProjectileStatePacket
    {
        PacketHeader header;
        unsigned int uId[16] = { 0 };
        float x[16] = { 0 };
        float y[16] = { 0 };
    };
};

