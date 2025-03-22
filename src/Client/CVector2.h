#ifndef CVECTOR2_H
#define CVECTOR2_H


template <typename T>
class CVector2
{
public:
    T x;
    T y;

public:
    CVector2() : x(0), y(0) {}
    CVector2(T x, T y) : x(x), y(y) {}

    CVector2<T> GetPosition() { return { x, y }; }

    void setX(T xValue) { x = xValue; }
    void setY(T yValue) { y = yValue; }

    CVector2 operator+(const CVector2& other) const
    {
        return CVector2(x + other.x, y + other.y);
    }

    CVector2 operator-(const CVector2& other) const
    {
        return CVector2(x - other.x, y - other.y);
    }

    CVector2 operator*(T scalar) const
    {
        return CVector2(x * scalar, y * scalar);
    }

    CVector2 operator/(T scalar) const
    {
        if (scalar == 0)
            throw std::runtime_error("Division par zéro !");

        return CVector2(x / scalar, y / scalar);
    }

    CVector2& operator+=(const CVector2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    CVector2& operator-=(const CVector2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    CVector2& operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    CVector2& operator/=(T scalar)
    {
        if (scalar == 0)
            throw std::runtime_error("Division par zéro !");

        x /= scalar;
        y /= scalar;
        return *this;
    }

    bool operator==(const CVector2& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const CVector2& other) const
    {
        return !(*this == other);
    }

    T magnitude() const
    {
        return std::sqrt(x * x + y * y);
    }

    CVector2 normalized() const
    {
        T mag = magnitude();
        if (mag == 0)
            throw std::runtime_error("Impossible de normaliser un vecteur nul !");

        return *this / mag;
    }

    T dot(const CVector2& other) const
    {
        return x * other.x + y * other.y;
    }
};

typedef CVector2<int> Vec2i;
typedef CVector2<float> Vec2f;

#endif // !CVECTOR2_H