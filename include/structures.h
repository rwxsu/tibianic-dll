#ifndef __STRUCTURESH__
#define __STRUCTURESH__

#include <stdint.h>

struct OutfitMasks_t {
	OutfitMasks_t(){
		head = 0;
		body = 0;
		legs = 0;
		feet = 0;
	}
	
	uint32_t head;
	uint32_t body;
	uint32_t legs;
	uint32_t feet;
};//__attribute__((packed));

struct Outfit_t {
	Outfit_t(){
		type = 0;
	}
	
	uint32_t type;
	OutfitMasks_t masks;
};//__attribute__((packed));

struct Creature_t {
	uint32_t id;
	char name[32];
	uint32_t x;
	uint32_t y;
	uint32_t z;
	
	uint32_t u1;
	uint32_t u2;
	uint32_t u3;
	uint32_t u4;
	uint32_t u5;
	uint32_t u6;
	
	uint32_t walking;
	uint32_t dir;	
	
	uint32_t u8;
	uint32_t u9;
	uint32_t u10;
	uint32_t u11;
	
	uint32_t outfit;
	uint32_t head;
	uint32_t body;
	uint32_t legs;
	uint32_t feet;
	
	uint32_t light;
	uint32_t lightcolor;
	
	uint64_t blacksquare;
	
	uint32_t hp;
	uint32_t speed;
	uint32_t visible;
	uint32_t skull;
	uint32_t party;
	
	uint32_t u12;
};//__attribute__((packed));

struct Object_t {
	uint32_t objectId;
	uint32_t objectData;
	uint32_t objectDataEx;
};//__attribute__((packed));

struct Tile_t {
	uint32_t amount;
	Object_t objects[14];
};//__attribute__((packed));

struct Map_t {
	//Tile_t tiles[2016];
	Tile_t tiles[18][14][8];
};//__attribute__((packed));

struct Container_t {
	uint32_t isAvailable;
	uint32_t itemId;
	uint32_t u2;
	uint32_t u3;
	char name[32];
	uint32_t capacity;
	uint32_t hasparent;
	uint32_t amount;
	Object_t objects[36];
};//__attribute__((packed));

struct Vip_t {
	uint32_t guid;
	char name[30];
	uint32_t online;
	
	uint32_t u1;
	uint16_t u2;
};//__attribute__((packed));

struct Viplist_t {
	Vip_t list[100];
};//__attribute__((packed));

struct Battlelist_t {
	Creature_t creatures[150];
};//__attribute__((packed));

struct Point_t {
	uint32_t x;
	uint32_t y;
};//__attribute__((packed));

struct Color_t {
	Color_t(uint8_t _red, uint8_t _green, uint8_t _blue){
		red = _red;
		green = _green;
		blue = _blue;
	}
	
	Color_t(){
		red = 0;
		green = 0;
		blue = 0;
	}
	
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};//__attribute__((packed));

struct Color {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
	
	Color(){
		red = green = blue = alpha = 255;
	}
	
	Color(int r, int g, int b, int a = 255){
		red = r;
		green = g;
		blue = b;
		alpha = a;
	}
	
	Color(float r, float g, float b, float a = 1.0f){
		red = r * 255.0f;
		green = g * 255.0f;
		blue = b * 255.0f;
		alpha = a * 255.0f;
	}

	const float rF() const {
		return (float)red / 255.0f;
	}

	const float gF() const {
		return (float)green / 255.0f;
	}

	const float bF() const {
		return (float)blue / 255.0f;
	}

	const float aF() const {
		return (float)alpha / 255.0f;
	}
	
	const uint32_t argb() const {
		return (uint32_t)alpha << 24 | (uint32_t)red << 16 | (uint32_t)green << 8 | (uint32_t)blue;
	}
	
	bool operator==(const Color &rhs){
		return red == rhs.red && green == rhs.green && blue == rhs.blue && alpha == rhs.alpha;
	}
	
	friend bool operator==(const Color &lhs, const Color &rhs){
		return lhs.red == rhs.red && lhs.green == rhs.green && lhs.blue == rhs.blue && lhs.alpha == rhs.alpha;
	}
};

struct Label_t {
	Label_t(){
		font = 1;
		red = green = blue = 255;
		align = 0;
	}
	
	Label_t(int nFont, int nRed, int nGreen, int nBlue, int nAlign){
		font = nFont;
		red = nRed;
		green = nGreen;
		blue = nBlue;
		align = nAlign;
	}
	
	uint32_t font;
	uint32_t red;
	uint32_t green;
	uint32_t blue;
	uint32_t align;
};//__attribute__((packed));

#endif
