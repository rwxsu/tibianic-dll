#ifndef __GUIH__
#define __GUIH__

#include <vector>

#include "guiwindow.h"
#include "position.h"

class GUI {
	public:
		GUI();
		
		void draw(uint32_t surface);
		void tick(uint32_t ticks);
		
		bool hasDialog();
		void setDialog(GUIWindow* dialog);
		GUIWindow* getDialog();
		
		/* Static method for nulling current dialog */
		static void nullDialog(Button* button, void* caller);
		static void pushDialog(Button* button, void* dialog);
		
		void pushElement(GUIElement* element);
		void releaseElement(GUIElement* element);
		void releaseElements();
		
		void pushWindow(GUIWindow* window);
		void releaseWindow(GUIWindow* window);
		void releaseWindows();
		
		void onResolution();
		bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);
		bool onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input);
	private:
		std::vector<GUIWindow*> m_windows;
		std::vector<GUIElement*> m_elements;
		
		Position m_click;
		
		GUIWindow* m_drag;
		GUIWindow* m_dialog;
};

#endif
