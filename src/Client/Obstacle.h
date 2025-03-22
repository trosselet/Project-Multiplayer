#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "Entity.h"

class Obstacle : public Entity
{
public:
    Obstacle();
    ~Obstacle();

    void SetPosition(float x, float y, float ratioX = 0.5f, float ratioY = 0.5f) override;

    Vec2f GetPosition(float ratioX = 0.5f, float ratioY = 0.5f) override;

    void OnUpdate() override;
    void OnCollision(Entity* other) override;
    void OnInit() override;
    void OnUnInit() override;

private:
    float Lerp(float a, float b, float t);

private:
    float mLerpTime;
    float mLerpDuration;
    float mLerpDirection;
    float r, g, b;
};

#endif