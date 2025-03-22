#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Entity.h"

class Projectile : public Entity
{
public:
	Projectile();
	~Projectile();

	void SetPosition(float x, float y, float ratioX = 0.5f, float ratioY = 0.5f) override;

	Vec2f GetPosition(float ratioX = 0.5f, float ratioY = 0.5f) override;

	void GoToDirection(float x, float y, float speed = 1.0f);

	void OnUpdate() override;
	void OnCollision(Entity* other) override;
	void OnInit() override;
	void OnUnInit() override;

private:


};

#endif