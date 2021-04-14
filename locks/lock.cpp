#include "lock.h"

Lock::Lock(){
	m_mutex = CreateMutex(NULL, FALSE, NULL);
}

Lock::~Lock(){
	CloseHandle(m_mutex);
}

DWORD Lock::lock(){
	return WaitForSingleObject(m_mutex, INFINITE);
}

bool Lock::unlock(){
	return ReleaseMutex(m_mutex);
}

ScopedLock::ScopedLock(Lock& lock){
	m_lock = &lock;
	lock.lock();
}

ScopedLock::~ScopedLock(){
	m_lock->unlock();
}
