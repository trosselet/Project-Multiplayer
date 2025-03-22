#include "pch.h"
#include "Server.h"

Server::Server() : mNextUid(1)
{
}

Server::~Server()
{
    DeleteCriticalSection(&mCriticalSection);
}

void Server::Start(unsigned short port)
{
    InitializeCriticalSection(&mCriticalSection);

    if (!mSocket.Init(port))
    {
        std::cerr << "Failed to initialize server socket" << std::endl;
        return;
    }

    CreateThread(nullptr, 0, &Server::ReceiveThread, this, 0, nullptr);
}

void Server::HandlePacket(const char* packet, const sockaddr_in& from)
{
    const CSockets::PacketHeader* header = reinterpret_cast<const CSockets::PacketHeader*>(packet);

    std::cout << "Packet type: " << header->type << std::endl;

    switch (header->type)
    {
    case CSockets::CONNECTION_REQUEST:
    {
        CSockets::ConnectionRequestPacket request;
        memcpy(&request, packet, sizeof(CSockets::ConnectionRequestPacket));
        HandleConnectionRequest(&request, from);
        break;
    }
    case CSockets::PLAYER_POSITION:
    {
        CSockets::PlayerStatePacket position;
        memcpy(&position, packet, sizeof(CSockets::PlayerStatePacket));
        HandlePlayerPosition(&position, from);
        break;
    }
    case CSockets::PLAYER_ACTION:
    {
        CSockets::PlayerActionPacket action;
        memcpy(&action, packet, sizeof(CSockets::PlayerActionPacket));
        HandlePlayerAction(&action, from);
        break;
    }
    case CSockets::PLAYER_DISCONNECT:
        HandlePlayerDisconnect(header->uid, from);
        break;

       case CSockets::PROJECTILE_POSITION:
        {
            CSockets::ProjectilePositionPacket projectilePosition;
            memcpy(&projectilePosition, packet, sizeof(CSockets::ProjectilePositionPacket));
            HandleProjectilePosition(&projectilePosition, from);
            break;
        }
    default:
        std::cout << "Unknown packet type received: " << header->type << std::endl;
        break;
    }
}

void Server::HandleConnectionRequest(const CSockets::ConnectionRequestPacket* request, const sockaddr_in& from)
{
    unsigned int uid = mNextUid++;
    mClients[uid] = from;

    PlayerInfo playerInfo = { uid, request->playerName, 3, 400.0f, 300.0f };
    mPlayers[uid] = playerInfo;

    std::cout << "Connection request from player: " << request->playerName << std::endl;
    std::cout << "Assigning UID: " << uid << std::endl;

    CSockets::ConnectionAcceptedPacket response;
    response.header.type = CSockets::CONNECTION_ACCEPTED;
    response.header.uid = uid;
    response.autoUid = uid;

    char buffer[sizeof(response)];
    memcpy(buffer, &response, sizeof(response));
    mSocket.SendTo("127.0.0.1", ntohs(from.sin_port), buffer, sizeof(buffer));

    BroadcastGameState();
}

void Server::HandlePlayerPosition(const CSockets::PlayerStatePacket* position, const sockaddr_in& from)
{
    unsigned int uid = position->header.uid;

    PlayerInfo& player = mPlayers[uid];
    player.x = position->x;
    player.y = position->y;
    player.hp = position->pv;

    std::cout << "Updated position for UID: " << uid
        << " to (" << player.x << ", " << player.y << ")" << std::endl;

    BroadcastGameState();
}

void Server::HandlePlayerAction(const CSockets::PlayerActionPacket* action, const sockaddr_in& from)
{
    std::cout << "Player Action Packet: " << std::endl;
    std::cout << "  UID: " << action->header.uid << std::endl;
    std::cout << "  Is Shooting: " << (action->isShooting ? "true" : "false") << std::endl;


    BroadcastGameState();
}

void Server::HandlePlayerDisconnect(unsigned int uid, const sockaddr_in& from)
{
    std::cout << "Player Disconnect Packet: " << std::endl;
    std::cout << "  UID: " << uid << " has disconnected." << std::endl;

    mClients.erase(uid);
    mPlayers.erase(uid);

    BroadcastGameState();
}

void Server::HandleProjectilePosition(const CSockets::ProjectilePositionPacket* position, const sockaddr_in& from)
{
    unsigned int uid = position->header.uid;

    ProjInfo& proj = mProjectiles[uid];
    proj.x = position->x;
    proj.y = position->y;

    BroadCastProjectilesStates();
}

void Server::BroadcastGameState()
{
    CSockets::GameStatePacket gameState;
    gameState.header.type = CSockets::GAME_STATE;

    std::cout << "Broadcasting Game State" << std::endl;

    int index = 0;
    for (const auto& [uid, player] : mPlayers)
    {
        gameState.playersScores[index] = uid;
        gameState.hpBar[index] = player.hp;
        gameState.x[index] = player.x;
        gameState.y[index] = player.y;
        ++index;
    }

    char buffer[sizeof(gameState)];
    memcpy(buffer, &gameState, sizeof(gameState));

    for (const auto& [uid, clientAddr] : mClients)
    {
        if (mSocket.SendTo("127.0.0.1", ntohs(clientAddr.sin_port), buffer, sizeof(buffer)))
        {
            std::cout << "SendTo                  TTTTTTTTTTTTTTTTTTTTTTTTT" << std::endl;
        }
        else
        {
            std::cout << "NotSendTo                  TTTTTTTTTTTTTTTTTTTTTTTTT" << std::endl;
        }
    }
}

void Server::BroadCastProjectilesStates()
{
    CSockets::ProjectileStatePacket projState;
    projState.header.type = CSockets::PROJECTILE_STATE;

    std::cout << "Broadcasting projectiles State" << std::endl;

    int index = 0;
    for (const auto& [uid, proj] : mProjectiles)
    {
        projState.uId[index] = uid;
        projState.x[index] = proj.x;
        projState.y[index] = proj.y;
        ++index;
    }

    char buffer[sizeof(projState)];
    memcpy(buffer, &projState, sizeof(projState));

    for (const auto& [uid, clientAddr] : mClients)
    {
        if (mSocket.SendTo("127.0.0.1", ntohs(clientAddr.sin_port), buffer, sizeof(buffer)))
        {
            std::cout << "SendTo                  TTTTTTTTTTTTTTTTTTTTTTTTT" << std::endl;
        }
        else
        {
            std::cout << "NotSendTo                  TTTTTTTTTTTTTTTTTTTTTTTTT" << std::endl;
        }
    }
}
