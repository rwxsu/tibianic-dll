#ifndef __PAINTERH__
#define __PAINTERH__

#include <iostream>
#include <stdint.h>

#include "const.h"
#include "functions.h"

typedef void _DrawSkin(int surface, int x, int y, int width, int height, int skin, int dx, int dy);
typedef void _DrawItem(int surface, int x, int y, int size, int itemId, int itemData1, int itemData2, int edgeRed, int edgeGreen, int edgeBlue, int clipX, int clipY, int clipWidth, int clipHeight, int textFont, int textRed, int textGreen, int textBlue, int textAlign, char textForce);
typedef void _DrawCreature(int surface, int x, int y, int size, int outfitId, int* outfitColors, int edgeRed, int edgeGreen, int edgeBlue, int clipX, int clipY, int clipWidth, int clipHeight);
typedef void _DrawText(int surface, int x, int y, int font, int red, int green, int blue, char* text, int align);
typedef void _DrawRectangle(int surface, int x, int y, int width, int height, int red, int green, int blue);
typedef void _DrawHollowRectangle(int surface, int16_t x, int16_t y, uint16_t width, uint16_t height, uint8_t thickness, int red, int green, int blue);
typedef void _DrawBorders(int surface, int x, int y, int width, int height);

class Painter {
	public:
		static Color getOutfitColor(int color);
		
		static _DrawSkin drawSkin;
		static _DrawItem drawItem;
		static _DrawCreature drawCreature;
		static _DrawText drawText;
		static _DrawRectangle drawRectangle;
		static _DrawHollowRectangle drawHollowRectangle;
		static _DrawBorders drawBorders;
	protected:
		Painter(){};
};

#endif
