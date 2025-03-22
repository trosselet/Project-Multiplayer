#ifndef THREAD_H
#define THREAD_H

#include <Windows.h>

class Thread
{
public:
	void Start();
	static DWORD StacitThreadStart(void* param);
	virtual void OnEnter() = 0;
};

#endif