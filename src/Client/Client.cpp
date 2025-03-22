#include "pch.h"
#include "Client.h"
#include "CWindow.h"
#include "CInput.h"
#include "Entity.h"
#include "Player.h"
#include "Projectile.h"
#include "CRandoms.h"
#include "ConnectDisplay.h"
#include "Obstacle.h"

#pragma comment(lib, "winmm.lib")

Client::Client() : mRunning(false), mpSocket(nullptr), mpWindow(nullptr), m_elapsedTime(0), m_time(0),
mAccumulatedDt(0), mControlledPlayerUid(0), mCriticalSection(), mDeltaTime(0), mServerAddress(), mCameraPosition({ 0.0f, 0.0f }),
mInput(nullptr), mIp(0)
{
}

Client::~Client()
{
	DeleteCriticalSection(&mCriticalSection);

	delete mpWindow;
	delete mInput;

	for (auto& player : mPlayers)
	{
		delete player.second;
	}

	for (auto& proj : mProjectiles)
	{
		delete proj.second;
	}

	for (auto& obstacle : mObstacles)
	{
		delete obstacle.second;
	}
}

bool Client::Connect(const char* serverIp, unsigned int port, const char* playerName)
{
	InitializeCriticalSection(&mCriticalSection);

	mpSocket = new CSockets();

	if (!mpSocket->Init(0))
	{
		std::cerr << "Failed to initialize client socket" << std::endl;
		return false;
	}

	mIp = serverIp;

	mServerAddress.sin_family = AF_INET;
	mServerAddress.sin_port = htons(port);
	inet_pton(AF_INET, mIp, &mServerAddress.sin_addr);


	return SendConnectionRequest(playerName);
}


void Client::Run()
{
	mRunning = true;

	CreateThread(nullptr, 0, &Client::ReceiveThread, this, 0, nullptr);

	mpWindow = new CWindow();
	mpWindow->Initialize(800, 600, "Multiplayer Game Client");

	mInput = new CInput();
	mInput->Init();

	InitializeObstacles();

	while (mpWindow->IsOpen())
	{
		if (UpdateTime())
		{
			mpWindow->HandleEvent();

			SetDeltaTime(m_elapsedTime);

			Update();

			Draw();
		}
	}

	mRunning = false;
}

bool Client::UpdateTime()
{
	static DWORD previousTime = timeGetTime();
	DWORD currentTime = timeGetTime();
	DWORD elapsedSysTime = currentTime - previousTime;

	if (elapsedSysTime == 0)
		return false;

	m_elapsedTime = elapsedSysTime / 1000.0f;

	float maxDeltaTime = 0.04f;

	if (m_elapsedTime > maxDeltaTime)
	{
		m_elapsedTime = maxDeltaTime;
	}

	previousTime = currentTime;
	m_time += m_elapsedTime;
	return true;
}

bool Client::SendConnectionRequest(const char* playerName)
{

	CSockets::ConnectionRequestPacket request;
	request.header.type = CSockets::CONNECTION_REQUEST;
	request.header.uid = 0;
	strncpy_s(request.playerName, playerName, sizeof(request.playerName));

	char buffer[40];
	memcpy(buffer, &request.header.type, 4);
	memcpy(buffer + 4, &request.header.uid, 4);
	memcpy(buffer + 8, &request.playerName, 32);


	return mpSocket->SendTo(mIp, ntohs(mServerAddress.sin_port), buffer, sizeof(buffer));
}

void Client::HandleServerPacket(const void* packet)
{
	const CSockets::PacketHeader* header = static_cast<const CSockets::PacketHeader*>(packet);

	switch (header->type)
	{
	case CSockets::CONNECTION_ACCEPTED:
	{
		const CSockets::ConnectionAcceptedPacket* response = static_cast<const CSockets::ConnectionAcceptedPacket*>(packet);
		mControlledPlayerUid = response->autoUid;
		std::cout << "Connected! Assigned UID: " << mControlledPlayerUid << std::endl;

		Player* controlledPlayer = CreatePlayer();
		controlledPlayer->SetId(mControlledPlayerUid);
		controlledPlayer->SetColor(0, 255, 0, 255);

		CRandoms randoms;
		int randomX = randoms.GetRandomInt(0, WIDTH - controlledPlayer->GetWidth());
		int randomY = randoms.GetRandomInt(0, HEIGHT - controlledPlayer->GetHeight());
		mCameraPosition.x = randomX;
		mCameraPosition.y =	randomY;

		mPlayers[mControlledPlayerUid] = controlledPlayer;

		SendPlayerState(mCameraPosition);
		UpdateObstacleState();

		break;
	}
	case CSockets::GAME_STATE:
	{
		const CSockets::GameStatePacket* gameState = static_cast<const CSockets::GameStatePacket*>(packet);
		UpdateGameState(gameState);
		break;
	}
	case CSockets::PROJECTILE_STATE:
	{
		const CSockets::ProjectileStatePacket* projectileState = static_cast<const CSockets::ProjectileStatePacket*>(packet);
		UpdateProjectileState(projectileState);
		break;
	}
	default:
		std::cerr << "Unknown packet type received" << std::endl;
		break;
	}
}

void Client::FixedUpdate()
{
	HandleInput();

	mInput->Update();

	if (!mPlayers.empty())
	{
		if (mPlayers[mControlledPlayerUid] != nullptr)
		{
			mPlayers[mControlledPlayerUid]->OnUpdate();

			if (mPlayers[mControlledPlayerUid]->GetPv() <= 0 && mPlayers[mControlledPlayerUid]->GetRespawnTimer() >= 3.0f)
			{
				RespawnPlayer();
				mPlayers[mControlledPlayerUid]->SetRespawnState(false);
			}

			SendPlayerState(mCameraPosition);
		}
	}

	if (!mProjectiles.empty())
	{
		for (auto& proj : mProjectiles)
		{
			if (proj.second != nullptr)
			{
				proj.second->OnUpdate();
			}
		}
	}

	if (mProjectiles.find(mControlledPlayerUid) != mProjectiles.end())
	{
		SendProjectilePosition();
	}

	for (auto& obstacle : mObstacles)
	{
		if (obstacle.second != nullptr)
		{
			obstacle.second->OnUpdate();
		}
	}


	//----------------------------------------------COLLISION----------------------------------------------------//

	//Player Player
	for (auto& player : mPlayers)
	{
		for (auto& _player : mPlayers)
		{
			if (_player.first != player.first && player.second->IsColliding(_player.second))
			{
				if (player.second != NULL || _player.second != NULL)
				{
					player.second->OnCollision(_player.second);
					_player.second->OnCollision(player.second);
				}
			}
		}

	//Player Obstacle

		for (auto& obstacle : mObstacles)
		{
			if (player.second->IsColliding(obstacle.second))
			{
				player.second->OnCollision(obstacle.second);
				obstacle.second->OnCollision(player.second);
			}
		}

	//Player world border

		player.second->SetPosition(
			player.second->Clamp(player.second->GetPosition().x, -WIDTH + player.second->GetWidth() / 2, WIDTH * 2 - player.second->GetWidth() / 2),
			player.second->Clamp(player.second->GetPosition().y, -HEIGHT + player.second->GetHeight() / 2, HEIGHT * 2 - player.second->GetHeight() / 2));

		mCameraPosition =
		{
					player.second->Clamp(mCameraPosition.x, -WIDTH + player.second->GetWidth() / 2, WIDTH * 2 - player.second->GetWidth() / 2),
					player.second->Clamp(mCameraPosition.y, -HEIGHT + player.second->GetHeight() / 2, HEIGHT * 2 - player.second->GetHeight() / 2)
		};
	}

	//Projectile Player
	for (auto& proj : mProjectiles)
	{
		for (auto& player : mPlayers)
		{
			if (player.second->GetPv() > 0)
			{
				if (proj.second->IsColliding(player.second))
				{
					proj.second->OnCollision(player.second);
					player.second->OnCollision(proj.second);
				}
			}
		}
	//Projectile Projectile
	
		for (auto& _proj : mProjectiles)
		{
			if (_proj.first != proj.first && proj.second->IsColliding(_proj.second))
			{
				proj.second->OnCollision(_proj.second);
				_proj.second->OnCollision(proj.second);
			}
		}

	//Projectile Obstacle

		for (auto& obstacle : mObstacles)
		{
			if (obstacle.second->GetPv() > 0)
			{
				if (proj.second->IsColliding(obstacle.second))
				{
					proj.second->OnCollision(obstacle.second);
					obstacle.second->OnCollision(proj.second);
				}
			}
		}
	}

	UpdateObstacleState();
	
}

Client* Client::Get()
{
	static Client mInstance;

	return &mInstance;
}

void Client::UpdateGameState(const CSockets::GameStatePacket* gameState)
{
	for (int i = 0; i < 16; i++)
	{
		if (gameState->playersScores[i] == 0)
			continue;

		if (mPlayers.find(i + 1) != mPlayers.end())
		{
			if (mPlayers[i + 1] != mPlayers[mControlledPlayerUid])
			{
				mPlayers[i + 1]->SetAbsolutePosition(gameState->x[i], gameState->y[i]);

				float offsetX = gameState->x[i] - mCameraPosition.x;
				float offsetY = gameState->y[i] - mCameraPosition.y;

				mPlayers[i + 1]->SetPosition(mPlayers[mControlledPlayerUid]->GetPosition().x + offsetX,
					mPlayers[mControlledPlayerUid]->GetPosition().y + offsetY);

				mPlayers[i + 1]->SetPv(gameState->hpBar[i]);
			}
		}
		else
		{
			Player* player = new Player();
			player->Init();
			player->SetPosition(gameState->x[i], gameState->y[i]);
			player->SetId(gameState->playersScores[i]);
			player->SetColor(255, 0, 0, 255);
			player->SetPv(gameState->hpBar[i]);
			mPlayers[gameState->playersScores[i]] = player;
		}
	}
}

void Client::UpdateProjectileState(const CSockets::ProjectileStatePacket* projState)
{
	for (int i = 0; i < 16; i++)
	{
		if (projState->uId[i] == 0)
			continue;


		if (mProjectiles.find(i + 1) != mProjectiles.end())
		{
			if (mProjectiles.find(i + 1) != mProjectiles.find(mControlledPlayerUid))
			{
				mProjectiles[i + 1]->SetAbsolutePosition(projState->x[i], projState->y[i]);

				float offsetX = mPlayers[i + 1]->GetAbsolutePosition().x - mCameraPosition.x;
				float offsetY = mPlayers[i + 1]->GetAbsolutePosition().y - mCameraPosition.y;

				mProjectiles[i + 1]->SetPosition(projState->x[i] + offsetX, projState->y[i] + offsetY);
			}
		}
		else
		{
			Projectile* proj = new Projectile();
			proj->Init();
			proj->SetPosition(mPlayers[i + 1]->GetPosition().x, mPlayers[i + 1]->GetPosition().y);
			proj->SetId(projState->uId[i]);
			mProjectiles[projState->uId[i]] = proj;
		}
	}
}

void Client::UpdateObstacleState()
{
	for (auto& obstacle : mObstacles)
	{
		Vec2f offset =
		{
			obstacle.second->GetAbsolutePosition().x - mCameraPosition.x,
			obstacle.second->GetAbsolutePosition().y - mCameraPosition.y
		};

		obstacle.second->SetPosition(mPlayers[mControlledPlayerUid]->GetPosition().x + offset.x,
			mPlayers[mControlledPlayerUid]->GetPosition().y + offset.y);
	}
}

void Client::HandleInput()
{
	mpWindow->HandleEvent();

	int speed = 1;

	if (Client::Get()->GetControlledPlayerUid() == 1)
	{


		Vec2f direction = { 0, 0 };

		if (mInput->GetKeyState(VK_Q) == CInput::DOWN ||
			mInput->GetKeyState(VK_Q) == CInput::PUSH)
		{
			direction.x -= speed;
		}
		if (mInput->GetKeyState(VK_D) == CInput::DOWN ||
			mInput->GetKeyState(VK_D) == CInput::PUSH)
		{
			direction.x += speed;
		}
		if (mInput->GetKeyState(VK_Z) == CInput::DOWN ||
			mInput->GetKeyState(VK_Z) == CInput::PUSH)
		{
			direction.y -= speed;
		}
		if (mInput->GetKeyState(VK_S) == CInput::DOWN ||
			mInput->GetKeyState(VK_S) == CInput::PUSH)
		{
			direction.y += speed;
		}
		if (mInput->GetKeyState(VK_SPACE) == CInput::PUSH)
		{
			Vec2f mousePos = { (float)sf::Mouse::getPosition(mpWindow->GetWindow()).x, (float)sf::Mouse::getPosition(mpWindow->GetWindow()).y};

			std::cout << "X = " << mousePos.x << " Y = " << mousePos.y << std::endl;

			Projectile* projectile = CreateProjectile();
			projectile->SetId(mControlledPlayerUid);
			projectile->SetPosition(mPlayers[mControlledPlayerUid]->GetPosition(0, 0).x, mPlayers[mControlledPlayerUid]->GetPosition(0, 0).y);
			projectile->SetColor(255, 0, 255, 255);
			projectile->GoToDirection(mousePos.x, mousePos.y, 5);

			mProjectiles[mControlledPlayerUid] = projectile;

			Vec2f direction = {mousePos.x, mousePos.y};

			SendProjectilePosition();
		}

		mCameraPosition += direction;

		/*if (direction != Vec2f(0, 0))
		{
			
		}*/
	}
}

bool Client::SendPlayerState(Vec2f position)
{

	CSockets::PlayerStatePacket packet;
	packet.header.type = CSockets::PLAYER_POSITION;
	packet.header.uid = GetControlledPlayerUid();
	packet.x = position.x;
	packet.y = position.y;
	packet.pv = mPlayers[mControlledPlayerUid]->GetPv();

	return mpSocket->SendTo(mIp, ntohs(mServerAddress.sin_port), &packet, sizeof(packet));
}

bool Client::SendProjectilePosition()
{
	if (mProjectiles.find(mControlledPlayerUid) != mProjectiles.end())
	{
		Projectile* projectile = mProjectiles[mControlledPlayerUid];
		if (projectile && !projectile->ToDestroy())
		{
			CSockets::ProjectilePositionPacket packet;
			packet.header.type = CSockets::PROJECTILE_POSITION;
			packet.header.uid = mControlledPlayerUid;
			packet.x = projectile->GetPosition().x;
			packet.y = projectile->GetPosition().y;

			return mpSocket->SendTo(mIp, ntohs(mServerAddress.sin_port), &packet, sizeof(packet));
		}
	}
	return false;
}

void Client::Update()
{
	mAccumulatedDt += GetDeltaTime();
	while (mAccumulatedDt >= FIXED_DT)
	{
		FixedUpdate();
		mAccumulatedDt -= FIXED_DT;
	}

	for (auto it = mPlayers.begin(); it != mPlayers.end();)
	{
		if (it->second->ToDestroy())
		{
			mEntitiesToDestroy[it->second->GetId()] = it->second;

			it = mPlayers.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (auto it = mProjectiles.begin(); it != mProjectiles.end();)
	{
		if (it->second->ToDestroy())
		{
			mEntitiesToDestroy[it->second->GetId() + 16] = it->second;

			it = mProjectiles.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (auto it = mObstacles.begin(); it != mObstacles.end();)
	{
		if (it->second->ToDestroy())
		{
			mEntitiesToDestroy[it->second->GetId() + 32] = it->second;

			it = mObstacles.erase(it);
		}
		else
		{
			++it;
		}
	}

	//Destroy 
	for (auto& entity : mEntitiesToDestroy)
	{
		delete entity.second;
	}
	mEntitiesToDestroy.clear();
}

void Client::Draw()
{
	mpWindow->Clear();

	for (auto& obstacle : mObstacles)
	{
		if (obstacle.second != nullptr)
		{
			mpWindow->Draw(obstacle.second);
		}
	}

	for (auto& player : mPlayers)
	{
		if (player.second->GetPv() > 0)
		{
			mpWindow->Draw(player.second);

			DrawHealthBar(player.second);
		}
	}

	for (auto& proj : mProjectiles)
	{
		mpWindow->Draw(proj.second);
	}

	DrawMiniMap();

	mpWindow->Display();
}

Player* Client::CreatePlayer()
{
	Player* player = new Player();
	player->Init();
	mPlayers[GetControlledPlayerUid()] = player;
	return player;
}

Projectile* Client::CreateProjectile()
{
	Projectile* proj = new Projectile();
	proj->Init();
	mProjectiles[GetControlledPlayerUid()] = proj;
	return proj;
}

void Client::DrawMiniMap()
{
	const int miniMapWidth = 150;
	const int miniMapHeight = 150;

	const int miniMapX = WIDTH - miniMapWidth - 20;
	const int miniMapY = 20;

	sf::RectangleShape miniMapBackground(sf::Vector2f(miniMapWidth, miniMapHeight));
	miniMapBackground.setPosition(miniMapX, miniMapY);
	miniMapBackground.setFillColor(sf::Color(50, 50, 50, 200));

	mpWindow->DrawShape(miniMapBackground);

	const float gameWorldWidth = WIDTH * 4;
	const float gameWorldHeight = HEIGHT * 4;

	for (const auto& playerPair : mPlayers)
	{
		Player* player = playerPair.second;

		if (player->GetPv() > 0)
		{
			sf::CircleShape playerCircle(3);

			if (playerPair.first == mControlledPlayerUid)
			{
				playerCircle.setFillColor(sf::Color::Green);

				float playerX = (mCameraPosition.x + gameWorldWidth / 2) / gameWorldWidth * miniMapWidth;
				float playerY = (mCameraPosition.y + gameWorldHeight / 2) / gameWorldHeight * miniMapHeight;

				playerCircle.setPosition(miniMapX + playerX - playerCircle.getRadius(),
					miniMapY + playerY - playerCircle.getRadius());
			}
			else
			{
				playerCircle.setFillColor(sf::Color::Red);

				float playerX = (player->GetAbsolutePosition().x + gameWorldWidth / 2) / gameWorldWidth * miniMapWidth;
				float playerY = (player->GetAbsolutePosition().y + gameWorldHeight / 2) / gameWorldHeight * miniMapHeight;

				playerCircle.setPosition(miniMapX + playerX - playerCircle.getRadius(),
					miniMapY + playerY - playerCircle.getRadius());
			}

			mpWindow->DrawShape(playerCircle);
		}
	}

	sf::RectangleShape gameBoundary(sf::Vector2f(miniMapWidth, miniMapHeight));
	gameBoundary.setPosition(miniMapX, miniMapY);
	gameBoundary.setOutlineThickness(1.0f);
	gameBoundary.setOutlineColor(sf::Color::White);
	gameBoundary.setFillColor(sf::Color::Transparent);

	mpWindow->DrawShape(gameBoundary);
}

void Client::DrawHealthBar(Player* player)
{
	const float barWidth = 50.0f;
	const float barHeight = 5.0f;

	Vec2f playerPos = player->GetPosition();
	float barX = playerPos.x - (barWidth / 2);
	float barY = playerPos.y - player->GetHeight() - 10;

	float healthPercentage = static_cast<float>(player->GetPv()) / player->GetPvMax();
	float filledBarWidth = barWidth * healthPercentage;

	sf::RectangleShape healthBarBackground(sf::Vector2f(barWidth, barHeight));
	healthBarBackground.setPosition(barX, barY);
	healthBarBackground.setFillColor(sf::Color(255, 0, 0, 255));

	sf::RectangleShape healthBar(sf::Vector2f(filledBarWidth, barHeight));
	healthBar.setPosition(barX, barY);
	healthBar.setFillColor(sf::Color(0, 255, 0, 255));

	mpWindow->DrawShape(healthBarBackground);
	mpWindow->DrawShape(healthBar);
}

void Client::InitializeObstacles()
{
	std::map<int, Vec2f> obstaclePositions = {
		{0, Vec2f(150, 200)},
		{1, Vec2f(300, 250)},
		{2, Vec2f(450, 100)},
		{3, Vec2f(500, 300)},
		{4, Vec2f(200, 350)}
	};

	for (const auto& pair : obstaclePositions)
	{
		int index = pair.first;
		Vec2f position = pair.second;

		Obstacle* obstacle = new Obstacle();
		obstacle->Init();
		obstacle->SetId(index + 16);
		obstacle->OnUpdate();
		obstacle->SetPosition(position.x, position.y);
		obstacle->SetAbsolutePosition(position.x, position.y);

		mObstacles[index] = obstacle;
	}
}

void Client::RespawnPlayer()
{
	if (mPlayers.find(mControlledPlayerUid) != mPlayers.end())
	{
		Player* controlledPlayer = mPlayers[mControlledPlayerUid];

		CRandoms randoms;
		float randomX = randoms.GetRandomInt(0, WIDTH - controlledPlayer->GetWidth());
		float randomY = randoms.GetRandomInt(0, HEIGHT - controlledPlayer->GetHeight());

		Vec2f newPosition = { randomX, randomY };
		controlledPlayer->SetPv(controlledPlayer->GetPvMax());

		mCameraPosition = newPosition;

		SendPlayerState(mCameraPosition);
	}
}
