#include "timer.h"

uint64_t Timer::tickCount(){
	//return GetTickCount();
	return timeGetTime();
}
