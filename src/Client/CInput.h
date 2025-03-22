#ifndef CINPUT_H
#define CINPUT_H

class CInput
{
public:
    enum KeyState
    {
        NONE,
        PUSH,
        DOWN,
        UP
    };

    CInput();
    void Init();
    void Update();
    KeyState GetKeyState(int key);

private:
    KeyState mKeyStates[256];
    bool mPrevKeyStates[256];
};

#endif