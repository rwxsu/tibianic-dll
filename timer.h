#ifndef __TIMERH__
#define __TIMERH__

#include <iostream>
#include <windows.h>

class Timer {
	public:
		static uint64_t tickCount();
	private:
		Timer(){};
};

#endif
