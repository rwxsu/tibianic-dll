#ifndef __GUIWINDOWH__
#define __GUIWINDOWH__

#include <iostream>
#include <stdint.h>

#include "guiparent.h"
#include "functions.h"
#include "buttons.h"
#include "lock.h"
#include "position.h"
#include "buttons.h"

class GUIWindow : public GUIParent {
	friend class GUI;
	
	public:
		/* Static functions */
		static void closeWindow(Button* button, void* lpParam);
		static void minimizeWindow(Button* button, void* lpParam);
		
		/* Constructor */
		GUIWindow(std::string caption, int x, int y, int width, int height, bool minimize = false, bool close = false);

		/* Change resolution event */
		void onResolution();
		
		/* Input events */
		virtual bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);
		virtual bool onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input);
		
		/* Bottom buttons in dialog */
		Button* pushButton(std::string caption, void (*callback)(Button*, void* lParam), void* param, int x, int y, bool bistate = false, bool mini = false);
		
		/* Virtual draw method from GUIElement */
		void draw(int surface);
		
		/* Virtual functions to set width and height of the element - the window cannot be resized once created */
		void setWidth(int width){ };
		void setHeight(int height){ };
	protected:
		int32_t getButtonNextX();
		int32_t getButtonNextY();
	private:
		virtual void drawSelf(int surface){ };
	protected:
		bool m_minimize;
};

#endif
