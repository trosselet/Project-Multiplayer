#ifndef ENTITY_H
#define ENTITY_H

namespace sf
{
	class Shape;
	class Color;
}


template <typename T>
class CVector2;

enum class EntityType
{
	Rectangle,
	Circle
};

enum class RectangleType
{
	Player,
	Obstacle
};

class Entity
{
protected:

	Vec2f mPosition;
	Vec2f mAbsolutePosition;
	Vec2f mDirection;
	float mSpeed;
	bool mToDestroy;
	unsigned int mId;
	int mPv;
	int mPvMax;

	EntityType mType;
	RectangleType mRectangleType;
	float mWidth, mHeight;
	float mRadius;

	sf::Color mColor;
	sf::Shape* mpShape;

protected:
	virtual void SetPosition(float x, float y, float ratioX = 0.5f, float ratioY = 0.5f) = 0;
	virtual Vec2f GetPosition(float ratioX = 0.5f, float ratioY = 0.5f) = 0;

	virtual void OnCollision(Entity* other) = 0;


	virtual void OnUpdate() {};
	virtual void OnInit() {};
	virtual void OnUnInit() {};
	
public:
	EntityType GetType() const { return mType; }
	RectangleType GetRectangleType() const { return mRectangleType; }
	sf::Shape& GetShape() const { return *mpShape; }
	unsigned int GetId() const { return mId; }
	float GetSpeed() { return mSpeed; }
	float GetWidth() { return mWidth; }
	float GetHeight() { return mHeight; }
	float GetRadius() { return mRadius; }
	int GetPvMax() { return mPvMax; }
	int GetPv() { return mPv; }
	float GetDeltaTime();
	virtual sf::Shape& GetDrawable() const { return *mpShape; }
	bool ToDestroy() { return mToDestroy; }
	Vec2f GetAbsolutePosition() { return mAbsolutePosition; }


	void SetColor(int r, int g, int b, int a);
	void SetId(unsigned int id) { mId = id; }
	void SetSpeed(float speed) { mSpeed = speed; }
	void SetPv(int _pv) { mPv = _pv; }
	void SetAbsolutePosition(float _x, float _y) { mAbsolutePosition = {_x, _y}; }

	void TakeDamage(int _damage);
	bool IsColliding(Entity* other);
	void Init();


	virtual ~Entity();
	Entity();

private:
	void Update();
	void UnInit();
};

#endif // !ENTITY_H