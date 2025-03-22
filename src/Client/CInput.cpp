#include "pch.h"
#include "CInput.h"

CInput::CInput() : mKeyStates(), mPrevKeyStates()
{
}

void CInput::Init()
{
    for (int i = 0; i < 256; ++i)
    {
        mKeyStates[i] = NONE;
        mPrevKeyStates[i] = false;
    }
}

void CInput::Update()
{
    for (int i = 0; i < 256; ++i)
    {
        SHORT keyState = GetAsyncKeyState(i);

        if (keyState & 0x8000)
        {
            if (mPrevKeyStates[i])
                mKeyStates[i] = DOWN;
            else
                mKeyStates[i] = PUSH;
        }
        else
        {
            if (mPrevKeyStates[i])
                mKeyStates[i] = UP;
            else
                mKeyStates[i] = NONE;
        }

        mPrevKeyStates[i] = (keyState & 0x8000) != 0;
    }
}

CInput::KeyState CInput::GetKeyState(int key)
{
    if (key < 0 || key >= 256)
        return NONE;

    return mKeyStates[key];
}