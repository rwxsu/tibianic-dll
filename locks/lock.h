#ifndef __LOCKH__
#define __LOCKH__

#include <iostream>
#include <windows.h>

#include <functions.h>

class Lock {
	public:
		Lock();
		~Lock();
		
		DWORD lock();
		bool unlock();
	private:
		HANDLE m_mutex;
};

class ScopedLock {
	public:
		ScopedLock(Lock& lock);
		~ScopedLock();
	private:
		Lock* m_lock;
};

class ScopedCriticalSection {
	public:
		ScopedCriticalSection(int section) : m_section(section) {
			Tibia::EnterCriticalSection(m_section);
		}
		
		~ScopedCriticalSection(){
			Tibia::LeaveCriticalSection(m_section);
		}
	
	private:
		int m_section;
};

#endif
