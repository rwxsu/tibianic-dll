#ifndef __SKINH__
#define __SKINH__

#include <iostream>

#include "guielement.h"
#include "functions.h"
#include "tools.h"

class Skin : public GUIElement {
	public:
		Skin(int x, int y, int width, int height, int skinId);
		
		/* Virtual function replaced with our own */
		bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);
		
		/* Set skin id */
		void setSkinId(int skinId);
		
		/* Virtual draw function */
		void draw(int surface);
	private:
		int m_skinId;
};

#endif
