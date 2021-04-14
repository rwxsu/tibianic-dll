#ifndef __THREADH__
#define __THREADH__

#include <iostream>
#include <windows.h>

class Thread {
	public:
		Thread(WINAPI DWORD (*f)(void* p), LPVOID argv, bool execute = true);
		
		bool start();
		bool isRunning();
	private:
		DWORD WINAPI (*m_callback)(void* arg);
		LPVOID m_argument;
		
		bool m_executed;
};

#endif
