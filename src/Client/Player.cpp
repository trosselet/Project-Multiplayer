#include "pch.h"
#include "Player.h"
#include "Obstacle.h"
#include "CVector2.h"

Player::Player() : mRespawnTimer(0), mIsRespawning(false)
{
}

Player::~Player()
{
}

void Player::SetPosition(float x, float y, float ratioX, float ratioY)
{
	float adjustedX = x - (GetWidth() * ratioX);
	float adjustedY = y - (GetHeight() * ratioY);

	mpShape->setPosition(adjustedX, adjustedY);
}

void Player::SetRespawnState(bool state)
{
    mIsRespawning = state;
}

Vec2f Player::GetPosition(float ratioX, float ratioY)
{
	float originalX = mpShape->getPosition().x + (GetWidth() * ratioX);
	float originalY = mpShape->getPosition().y + (GetHeight() * ratioY);
	return Vec2f(originalX, originalY);
}

float Player::GetRespawnTimer()
{
    return mRespawnTimer;
}

void Player::OnUpdate()
{
	mPosition = { mpShape->getPosition().x, mpShape->getPosition().y };

    if (mPv <= 0)
    {
        mPv = 0;
        if (!mIsRespawning)
        {
            mRespawnTimer = 0.0f;
        }
        mIsRespawning = true;
    }

    if (mIsRespawning)
    {
        mRespawnTimer += GetDeltaTime();
    }

	/*std::cout << "ID = " << GetId() << " X = " << GetPosition().x << " Y = " << GetPosition().y << std::endl;*/
}

void Player::OnCollision(Entity* collidedWith)
{
    if (!collidedWith) return;

    if (collidedWith->GetType() == EntityType::Rectangle)
    {
        /*Vec2f collidedWithPosition = {0, 0};
        float collidedWithWidth = 0;
        float collidedWithHeight = 0;


        Vec2f position = GetPosition();
        float width = mWidth;
        float height = mHeight;

        if (collidedWith->GetRectangleType() == RectangleType::Player)
        {
            Player* player = dynamic_cast<Player*>(collidedWith);
            collidedWithPosition = player->GetPosition();
            collidedWithWidth = player->GetWidth();
            collidedWithHeight = player->GetHeight();
        }

        if (collidedWith->GetRectangleType() == RectangleType::Obstacle)
        {
            Obstacle* obstacle = dynamic_cast<Obstacle*>(collidedWith);
            collidedWithPosition = obstacle->GetPosition();
            collidedWithWidth = obstacle->GetWidth();
            collidedWithHeight = obstacle->GetHeight();
        }

        float deltaX = position.x - collidedWithPosition.x;
        float deltaY = position.y - collidedWithPosition.y;

        float overlapX = (width / 2 + collidedWithWidth / 2) - std::abs(deltaX);
        float overlapY = (height / 2 + collidedWithHeight / 2) - std::abs(deltaY);

        if (overlapX > 0 && overlapY > 0)
        {
            if (overlapX < overlapY)
            {
                if (deltaX > 0)
                {
                    SetPosition(collidedWithPosition.x + (collidedWithWidth / 2 + width / 2), GetPosition(0, 0).y, 0, 0);
                }
                else
                {
                    SetPosition(collidedWithPosition.x - (collidedWithWidth / 2 + width / 2), GetPosition(0, 0).y, 0, 0);
                }
            }
            else
            {
                if (deltaY > 0)
                {
                    SetPosition(GetPosition(0, 0).x, collidedWithPosition.y + (collidedWithHeight / 2 + height / 2), 0, 0);
                }
                else
                {
                    SetPosition(GetPosition(0, 0).x, collidedWithPosition.y - (collidedWithHeight / 2 + height / 2), 0, 0);
                }
            }
        }*/
    }
    
    if (collidedWith->GetType() == EntityType::Circle)
    {
        return;
    }
}

void Player::OnInit()
{
	mWidth = 20;
	mHeight = 10;
    mType = EntityType::Rectangle;
    mRectangleType = RectangleType::Player;
	mpShape = new sf::RectangleShape(sf::Vector2f(mWidth, mHeight));
	SetPosition(400, 300);
    mPvMax = 3;
	mPv = mPvMax;
	mPosition = GetPosition();
	mColor = sf::Color::Red;
	SetColor(mColor.r, mColor.g, mColor.b, mColor.a);
    mRespawnTimer = 0.0f;
    mIsRespawning = false;
}

void Player::OnUnInit()
{
	mPosition = { 0, 0 };
	mColor = sf::Color();
	mPv = 0;
	mWidth = 0;
	mHeight = 0;
    mRespawnTimer = 0.0f;
    mIsRespawning = false;
}

float Player::Clamp(float value, float min, float max)
{
    if (value < min)
    {
        return min;
    }

    if (value > max)
    {
        return max;
    }

    return value;
}
