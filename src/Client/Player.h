#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"

template <typename T>
class CVector2;

class Player : public Entity
{
public:
	Player();
	~Player();

	void SetPosition(float x, float y, float ratioX = 0.5f, float ratioY = 0.5f) override;
	void SetRespawnState(bool state);
	Vec2f GetPosition(float ratioX = 0.5f, float ratioY = 0.5f) override;
	float GetRespawnTimer();

	void OnUpdate() override;
	void OnCollision(Entity* other) override;
	void OnInit() override;
	void OnUnInit() override;

	float Clamp(float value, float min, float max);

private:
	float mRespawnTimer;
	bool mIsRespawning;
};

#endif
