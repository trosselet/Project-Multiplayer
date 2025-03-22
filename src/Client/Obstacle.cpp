#include "pch.h"
#include "Obstacle.h"
#include "CVector2.h"

Obstacle::Obstacle() : mLerpDirection(false), mLerpDuration(0), mLerpTime(0), r(0), g(0), b(0)
{
}

Obstacle::~Obstacle()
{
    OnUnInit();
}

void Obstacle::SetPosition(float x, float y, float ratioX, float ratioY)
{
    float adjustedX = x - (GetWidth() * ratioX);
    float adjustedY = y - (GetHeight() * ratioY);

    mpShape->setPosition(adjustedX, adjustedY);
}

Vec2f Obstacle::GetPosition(float ratioX, float ratioY)
{
    float originalX = mpShape->getPosition().x + (GetWidth() * ratioX);
    float originalY = mpShape->getPosition().y + (GetHeight() * ratioY);
    return Vec2f(originalX, originalY);
}

void Obstacle::OnUpdate()
{
    mPosition = { GetPosition().x, GetPosition().y};
    mLerpTime += GetDeltaTime();
    float t = mLerpTime / mLerpDuration;
    if (t > 1.0f) 
    {
        t = 1.0f;
    }

    if (r >= 255)
    {
        mLerpDirection = false;
    }
    else if (r <= 0)
    {
        mLerpDirection = true;
    }

    if (mLerpDirection)
    {
        r = Lerp(0, 255, t);
        g = Lerp(255, 0, t);
    }
    else
    {
        r = Lerp(255, 0, t);
        g = Lerp(0, 255, t);
    }
    

    SetColor(r, g, 0, 255);

    if (t >= 1.0f)
    {
        mLerpTime = 0;
    }

    if (mPv <= 0)
    {
        mToDestroy = true;
    }
}

void Obstacle::OnCollision(Entity* other)
{
}

void Obstacle::OnInit()
{
    mWidth = 10;
    mHeight = 50;

    mType = EntityType::Rectangle;
    mRectangleType = RectangleType::Obstacle;

    mpShape = new sf::RectangleShape(sf::Vector2f(mWidth, mHeight));

    mColor = sf::Color::Green;
    SetColor(mColor.r, mColor.g, mColor.b, mColor.a);

    mPvMax = 1;
    mPv = mPvMax;

    mLerpTime = 0;
    mLerpDuration = 1.0f;
    mLerpDirection = true;

    mPosition = GetPosition();
}

void Obstacle::OnUnInit()
{
    mPosition = { 0, 0 };
    mColor = sf::Color();
    mPv = 0;
    mWidth = 0;
    mHeight = 0;
    mLerpTime = 0;
    mLerpDuration = 0;
    mLerpDirection = false;

    if (mpShape)
    {
        delete mpShape;
        mpShape = nullptr;
    }
}

float Obstacle::Lerp(float a, float b, float t)
{
    return a + t * (b - a);
}
