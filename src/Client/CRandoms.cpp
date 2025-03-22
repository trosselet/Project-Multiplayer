#include "pch.h"
#include "CRandoms.h"
#include <cstdlib>

CRandoms::CRandoms()
{
    // Initialise le générateur de nombres aléatoires avec le temps actuel
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

int CRandoms::GetRandomInt(int min, int max)
{
    return min + (std::rand() % (max - min + 1));
}

float CRandoms::GetRandomFloat(float min, float max)
{
    return min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}
