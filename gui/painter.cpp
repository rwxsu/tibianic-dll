#include "painter.h"

_DrawSkin *DrawSkin = (_DrawSkin*)DRAW_SKIN_FUNCTION_ADDRESS;
_DrawItem *DrawItem = (_DrawItem*)DRAW_ITEM_FUNCTION_ADDRESS;
_DrawCreature *DrawCreature = (_DrawCreature*)DRAW_CREATURE_FUNCTION_ADDRESS;
_DrawText *PrintText = (_DrawText*)PRINT_TEXT_FUNCTION_ADDRESS;

Color Painter::getOutfitColor(int color){
	#define HSI_SI_VALUES 7
	#define HSI_H_STEPS 19

    if(color >= HSI_H_STEPS * HSI_SI_VALUES)
        color = 0;

    float loc1 = 0, loc2 = 0, loc3 = 0;
	if(color % HSI_H_STEPS != 0) {
        loc1 = color % HSI_H_STEPS * 1.0/18.0;
        loc2 = 1;
        loc3 = 1;

        switch(int(color / HSI_H_STEPS)) {
        case 0:
            loc2 = 0.25;
            loc3 = 1.00;
            break;
        case 1:
            loc2 = 0.25;
            loc3 = 0.75;
            break;
        case 2:
            loc2 = 0.50;
            loc3 = 0.75;
            break;
        case 3:
            loc2 = 0.667;
            loc3 = 0.75;
            break;
        case 4:
            loc2 = 1.00;
            loc3 = 1.00;
            break;
        case 5:
            loc2 = 1.00;
            loc3 = 0.75;
            break;
        case 6:
            loc2 = 1.00;
            loc3 = 0.50;
            break;
        }
    }
    else {
        loc1 = 0;
        loc2 = 0;
        loc3 =  1 - (float)color / HSI_H_STEPS / (float)HSI_SI_VALUES;
    }

	if(loc3 == 0){
		return Color(0, 0, 0);
	}

    if(loc2 == 0){
		return Color(loc3, loc3, loc3);
	}

	float red = 0, green = 0, blue = 0;

	if(loc1 < 1.0/6.0) {
        red = loc3;
        blue = loc3 * (1 - loc2);
        green = blue + (loc3 - blue) * 6 * loc1;
    }
    else if(loc1 < 2.0/6.0) {
        green = loc3;
        blue = loc3 * (1 - loc2);
        red = green - (loc3 - blue) * (6 * loc1 - 1);
    }
    else if(loc1 < 3.0/6.0) {
        green = loc3;
        red = loc3 * (1 - loc2);
        blue = red + (loc3 - red) * (6 * loc1 - 2);
    }
    else if(loc1 < 4.0/6.0) {
        blue = loc3;
        red = loc3 * (1 - loc2);
        green = blue - (loc3 - red) * (6 * loc1 - 3);
    }
    else if(loc1 < 5.0/6.0) {
        blue = loc3;
        green = loc3 * (1 - loc2);
        red = green + (loc3 - green) * (6 * loc1 - 4);
    }
    else {
        red = loc3;
        green = loc3 * (1 - loc2);
        blue = red - (loc3 - green) * (6 * loc1 - 5);
	}


	return Color(red, green, blue);
}

void Painter::drawSkin(int surface, int x, int y, int width, int height, int skin, int dx, int dy){
	DrawSkin(surface, x, y, width, height, skin, dx, dy);
}

void Painter::drawItem(int surface, int x, int y, int size, int itemId, int itemData1, int itemData2, int edgeRed, int edgeGreen, int edgeBlue, int clipX, int clipY, int clipWidth, int clipHeight, int textFont, int textRed, int textGreen, int textBlue, int textAlign, char textForce){
	DrawItem(surface, x, y, size, itemId, itemData1, itemData2, edgeRed, edgeGreen, edgeBlue, clipX, clipY, clipWidth, clipHeight, textFont, textRed, textGreen, textBlue, textAlign, textForce);
}

void Painter::drawCreature(int surface, int x, int y, int size, int outfitId, int* outfitColors, int edgeRed, int edgeGreen, int edgeBlue, int clipX, int clipY, int clipWidth, int clipHeight){
	DrawCreature(surface, x, y, size, outfitId, outfitColors, edgeRed, edgeGreen, edgeBlue, clipX, clipY, clipWidth, clipHeight);
}

void Painter::drawText(int surface, int x, int y, int font, int red, int green, int blue, char* text, int align){
	PrintText(surface, x, y, font, red, green, blue, text, align);
}

void Painter::drawRectangle(int surface, int x, int y, int width, int height, int red, int green, int blue){
	asm("mov %0, %%eax":: "r" (GET_PAINTER_FUNCTION_ADDRESS));
	asm("call *%eax");
	asm("mov %eax, %ecx");
	asm("mov (%ecx), %eax");
	asm("push %0":: "m" (blue));
	asm("push %0":: "m" (green));
	asm("push %0":: "m" (red));
	asm("push %0":: "m" (height));
	asm("push %0":: "m" (width));
	asm("push %0":: "m" (y));
	asm("push %0":: "m" (x));
	asm("push %0":: "m" (surface));
	asm("call *0x14(%eax)");
}

void Painter::drawHollowRectangle(int surface, int16_t x, int16_t y, uint16_t width, uint16_t height, uint8_t thickness, int red, int green, int blue){
	Painter::drawRectangle(surface, x, y, width, thickness, red, green, blue);
	Painter::drawRectangle(surface, x, y, thickness, height, red, green, blue);
	Painter::drawRectangle(surface, x + width - thickness, y, thickness, height, red, green, blue);
	Painter::drawRectangle(surface, x, y + height - thickness, width, thickness, red, green, blue);
}

void Painter::drawBorders(int surface, int x, int y, int width, int height){
	DrawSkin(surface, x, y, width, 1, 40, 0, 0); //top
	DrawSkin(surface, x, y, 1, height, 47/*27*/, 0, 0); //left
	DrawSkin(surface, x, y + height, width, 1, 41, 0, 0); //bottom
	DrawSkin(surface, x + width - 1, y + 1, 1, height - 1, 47, 0, 0); //right
}
