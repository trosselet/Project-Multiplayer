#include "pch.h"
#include "Entity.h"

#include "Client.h"

Entity::Entity() : mColor({0, 0, 0, 0}), mSpeed(0), mToDestroy(false), mId(0), mpShape(nullptr), mPosition(), mWidth(0), mHeight(0), mType(),
mPv(0), mPvMax(0)
{
}

float Entity::GetDeltaTime()
{
	return Client::Get()->GetDeltaTime();
}

void Entity::SetColor(int r, int g, int b, int a)
{
    mpShape->setFillColor(sf::Color(r, g, b, a));
}

void Entity::TakeDamage(int _damage)
{
    mPv -= _damage;
}

bool Entity::IsColliding(Entity* other)
{
    if (mType == EntityType::Rectangle && other->GetType() == EntityType::Rectangle)
    {
        sf::RectangleShape& rect1 = static_cast<sf::RectangleShape&>(GetShape());
        sf::RectangleShape& rect2 = static_cast<sf::RectangleShape&>(other->GetShape());

        return rect1.getGlobalBounds().intersects(rect2.getGlobalBounds());
    }
    else if (mType == EntityType::Circle && other->GetType() == EntityType::Circle)
    {
        Vec2f pos1 = GetPosition(0.0);
        Vec2f pos2 = other->GetPosition(0.0);
        float dist = (pos1 - pos2).magnitude();
        return dist < (mRadius + other->mRadius);
    }
    else if ((mType == EntityType::Rectangle && other->GetType() == EntityType::Circle) ||
        (mType == EntityType::Circle && other->GetType() == EntityType::Rectangle))
    {
        Entity* rectEntity = (mType == EntityType::Rectangle) ? this : other;
        Entity* circleEntity = (mType == EntityType::Circle) ? this : other;
        Vec2f circlePos = circleEntity->GetPosition();
        sf::RectangleShape& rect = static_cast<sf::RectangleShape&>(rectEntity->GetShape());
        float closestX = std::clamp(circlePos.x, rect.getPosition().x, rect.getPosition().x + rect.getSize().x);
        float closestY = std::clamp(circlePos.y, rect.getPosition().y, rect.getPosition().y + rect.getSize().y);
        float distanceX = circlePos.x - closestX;
        float distanceY = circlePos.y - closestY;
        return (distanceX * distanceX + distanceY * distanceY) < (circleEntity->mRadius * circleEntity->mRadius);
    }

    return false;
}

Entity::~Entity()
{
	delete mpShape;
}

void Entity::Init()
{
	OnInit();
}

void Entity::Update()
{
    

	OnUpdate();
}

void Entity::UnInit()
{
	OnUnInit();
}

void Entity::SetPosition(float x, float y, float ratioX, float ratioY)
{
	float adjustedX = x - (GetWidth() * ratioX);
	float adjustedY = y - (GetHeight() * ratioY);

	mpShape->setPosition(adjustedX, adjustedY);
}

Vec2f Entity::GetPosition(float ratioX, float ratioY)
{
	float originalX = mpShape->getPosition().x + (GetWidth() * ratioX);
	float originalY = mpShape->getPosition().y + (GetHeight() * ratioY);
	return Vec2f(originalX, originalY);
}