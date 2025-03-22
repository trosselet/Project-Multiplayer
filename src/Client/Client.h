#ifndef CLIENT_H
#define CLIENT_H

#define WIDTH 800
#define HEIGHT 600
#define FIXED_DT 0.01667f
#define VK_Z 0x5A
#define VK_Q 0x51
#define VK_S 0x53
#define VK_D 0x44

#include "CSockets.h"
#include <map>
#include <string>

namespace sf
{
    class RectangleShape;
}

class CWindow;
class CInput;
class Entity;
class Player;
class Projectile;
class Obstacle;

class Client
{
private:
    sockaddr_in mServerAddress;

    CWindow* mpWindow;

    const char* mIp;

    float m_elapsedTime, m_time;
    float mAccumulatedDt;
    float mDeltaTime;

    Vec2f mCameraPosition;
    CInput* mInput;

public:

    unsigned int mControlledPlayerUid;

    std::map<unsigned int, Player*> mPlayers;
    std::map<unsigned int, Projectile*> mProjectiles;
    std::map<unsigned int, Obstacle*> mObstacles;
    std::map<unsigned int, Entity*> mEntitiesToDestroy;

    CSockets* mpSocket;
    CRITICAL_SECTION mCriticalSection;
    bool mRunning;

public:
    Client();
    virtual ~Client();

    bool Connect(const char* serverIp, unsigned int port, const char* playerName);
    
    void Run();

    void HandleServerPacket(const void* packet);

    void FixedUpdate();

    static Client* Get();

    bool SendPlayerState(Vec2f position);
    
    bool SendProjectilePosition();

    float GetDeltaTime() const { return mDeltaTime; }

    unsigned int GetControlledPlayerUid() const { return mControlledPlayerUid; }

    Vec2f GetCameraPosition() const { return mCameraPosition; }

private:

    bool UpdateTime();

    bool SendConnectionRequest(const char* playerName);

    void UpdateGameState(const CSockets::GameStatePacket* gameState);

    void UpdateProjectileState(const CSockets::ProjectileStatePacket* projState);

    void UpdateObstacleState();

    void HandleInput();

    void Update();

    void Draw();

    void SetDeltaTime(float deltaTime) { mDeltaTime = deltaTime; }

    Player* CreatePlayer();

    Projectile* CreateProjectile();

    void DrawMiniMap();

    void DrawHealthBar(Player* player);

    void InitializeObstacles();

    void RespawnPlayer();

    static DWORD WINAPI ReceiveThread(LPVOID param)
    {
        Client* client = static_cast<Client*>(param);
        char buffer[1024];
        sockaddr_in from;

        while (client->mRunning)
        {
            if (client->mpSocket->ReceiveFrom(buffer, sizeof(buffer), from))
            {
                EnterCriticalSection(&client->mCriticalSection);
                client->HandleServerPacket(buffer);
                LeaveCriticalSection(&client->mCriticalSection);
            }
        }

        return 0;
    }
};




#endif // !CLIENT_H