#ifndef CRANDOMS_H
#define CRANDOMS_H

#include <ctime>

class CRandoms
{
public:
    CRandoms();

    int GetRandomInt(int min, int max);
    float GetRandomFloat(float min, float max);
};

#endif // !CRANDOMS_H
