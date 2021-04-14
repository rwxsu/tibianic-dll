#ifndef __POSITIONH__
#define __POSITIONH__

#include <iostream>
#include <stdint.h>

struct Position {
	Position(int32_t _x, int32_t _y, int32_t _z){
		x = _x;
		y = _y;
		z = _z;
	}
	
	Position(){
		x = 0;
		y = 0;
		z = 0;
	}

	bool operator==(const Position &right){
		if(x == right.x && y == right.y && z == right.z){
			return true;
		}
		
		return false;
	}

	friend bool operator<(const Position& l, const Position& p) {
		if(l.z < p.z)
			return true;
		if(l.z > p.z)
			return false;

		if(l.y < p.y)
			return true;
		if(l.y > p.y)
			return false;

		if(l.x < p.x)
			return true;
		if(l.x > p.x)
			return false;

		return false;
	}
	
	void clear(){
		x = y = z = 0;
	}
	
	int32_t x, y, z;
};

#endif
