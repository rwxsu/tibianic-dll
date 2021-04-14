#include "thread.h"

Thread::Thread(DWORD WINAPI (*f)(void* p), LPVOID argv, bool execute /*= true*/){
	m_callback = f;
	m_argument = argv;
	m_executed = false;
	
	if(execute){
		m_executed = CreateThread(NULL, 0, m_callback, m_argument, 0, NULL) != NULL;
	}
}

bool Thread::start(){
	if(!m_executed){
		m_executed = CreateThread(NULL, 0, m_callback, m_argument, 0, NULL) != NULL;
	}
}

bool Thread::isRunning(){
	return m_executed;
}

