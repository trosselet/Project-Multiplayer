#include "pch.h"
#include "Projectile.h"

Projectile::Projectile()
{
}

Projectile::~Projectile()
{
}

void Projectile::SetPosition(float x, float y, float ratioX, float ratioY)
{
	float adjustedX = x - (GetWidth() * ratioX);
	float adjustedY = y - (GetHeight() * ratioY);

	mpShape->setPosition(adjustedX, adjustedY);
}

Vec2f Projectile::GetPosition(float ratioX, float ratioY)
{
	float originalX = mpShape->getPosition().x + (GetWidth() * ratioX);
	float originalY = mpShape->getPosition().y + (GetHeight() * ratioY);

	return Vec2f(originalX, originalY);
}

void Projectile::GoToDirection(float x, float y, float speed)
{
	if (speed != 0)
		mSpeed = speed;

	Vec2f position = GetPosition();
	Vec2f direction = Vec2f(x - position.x, y - position.y);

	if (direction.normalized() == Vec2f(0, 0))
	{
		return;
	}

	mDirection = direction;
}

void Projectile::OnUpdate()
{
	float dt = GetDeltaTime();
	float distance = dt * mSpeed;
	Vec2f translation = { distance * mDirection.x, distance * mDirection.y };
	mpShape->move(translation.x, translation.y);

	std::cout << mId << "    " << mPosition.x << "   " << mPosition.y << std::endl;

	mPosition = { GetPosition() };

	if (ToDestroy())
	{
		std::cout << "ToDestroy" << std::endl;
	}
}

void Projectile::OnCollision(Entity* other)
{
	if (other->GetId() != mId)
	{
		mToDestroy = true;
		other->TakeDamage(1);
	}
}

void Projectile::OnInit()
{
	mRadius = 4;
	mType = EntityType::Circle;
	mpShape = new sf::CircleShape(mRadius);
	SetPosition(-1000, -1000);
	mPosition = GetPosition();
	mColor = sf::Color::Magenta;
	SetColor(mColor.r, mColor.g, mColor.b, mColor.a);
}

void Projectile::OnUnInit()
{
	mRadius = 0;
	mpShape = nullptr;
	mPosition = { 0, 0 };
	mColor = { 0, 0, 0, 0 };
}
