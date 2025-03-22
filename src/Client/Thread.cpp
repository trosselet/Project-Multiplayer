#include "pch.h"
#include "Thread.h"

void Thread::TStart()
{
	CreateThread(nullptr, 0, StacitThreadStart, this, 0, nullptr);
}

DWORD Thread::StacitThreadStart(void* param)
{
	Thread* pThread = static_cast<Thread*>(param);
	pThread->OnEnter();
	return 0;
}