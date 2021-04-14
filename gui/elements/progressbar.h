#ifndef __PROGRESSBARH__
#define __PROGRESSBARH__

#include <iostream>

#include "guielement.h"
#include "structures.h"

class ProgressBar : public GUIElement {
	public:
		ProgressBar(int x, int y, int width, int height, int min, int max, int red, int green, int blue);
		
		/* Basic progress bar functions */
		void step();
		void back();
		
		void step(int steps);
		void back(int steps);
		
		void pos(int position);
		
		/* Virtual draw function */
		void draw(int surface);
	private:
		Color_t m_color;
		
		int m_min;
		int m_max;
		int m_pos;
};

#endif
