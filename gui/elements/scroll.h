#ifndef __SCROLLH__
#define __SCROLLH__

#include "guiparent.h"
#include "const.h"
#include "buttons.h"
#include "tools.h"

#include <cmath>
#include <iostream>
#include <sstream>

#define SCROLL_ICON_DIMENSION 12

class Scroll : public GUIParent {
	public:
		Scroll(int x, int y, Scroll_t type, int min, int max, int length, double size, int step);
		
		/* Static functions for scroll */
		static void scrollUp(Button* button, void* lParam);
		static void scrollDown(Button* button, void* lParam);
		
		/* Virtual function replaced with our own */
		bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);
		
		int getPosition();	
		int getMax();	
		int getMin();
	
		void setMin(int min);
		void setMax(int max);
		void setSize(double size);
		void setLength(int lenValue);
		void setPosition(int position);
		
		/* Virtual draw function */
		void draw(int surface);
	protected:
		void update();
	private:
		Scroll_t m_type;
		
		int m_min;
		int m_max;
		
		int m_value;
		int m_length;
		double m_size;
		int m_step;
		
		int m_mark;
		int m_displacement;
		
		int m_click;
		bool m_hold;
		
		Button* m_upButton;
		Button* m_downButton;
};

#endif
