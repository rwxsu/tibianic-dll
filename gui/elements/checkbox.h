#ifndef __CHECKBOXH__
#define __CHECKBOXH__

#include <iostream>
#include <vector>
#include <algorithm>

#include "guielement.h"
#include "position.h"
#include "functions.h"
#include "tools.h"

class CheckBox : public GUIElement {
	public:
		CheckBox(int x, int y, int width, bool border = true);
		
		/* Override mouse events */
		bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);
		
		/* Set text of the CheckBox */
		void setText(std::string text);
		
		/* Basic functions */
		bool checked();
		void check();
		void uncheck();
		
		/* Override draw method */
		void draw(int surface);
	private:
		std::string m_text;
		
		bool m_checked;
		bool m_border;
};

#endif
