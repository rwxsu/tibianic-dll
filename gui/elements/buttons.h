#ifndef __BUTTONSH__
#define __BUTTONSH__

#define BUTTON_WIDTH 86
#define BUTTON_HEIGHT 20

#include "const.h"
#include "position.h"
#include "guielement.h"

#include <iostream>
#include <map>
#include <vector>
#include <stdint.h>
#include <windows.h>

class Button {
	friend class Buttons;
	
	public:
		Button(std::string caption, void (*function)(Button*, void*), void* lParam, int x, int y, bool bistate = false, bool gsmall = false);

		void draw(int nSurface, int x, int y);
		bool click();
		
		void enable();
		void disable();
		bool enabled();
		
		void activate();
		void deactivate();
		bool active();
		
		int getGraphics();
		
		void setCaption(std::string caption);
		void setGraphics(int id, int width, int height);
		void setActive(bool active);		
		void setPrintable(bool printable);		
		void setPosition(int x, int y);
	protected:
		std::string m_caption;
		
		bool m_bistate;
		bool m_active;
		bool m_enabled;
		bool m_text;
		void (*m_callback)(Button*, void*);
		void* m_callbackParam;
		
		Position m_position;
		int m_width, m_height;
		int m_skin;
};

class Buttons : public GUIElement {
	public:
		Buttons(int x, int y);
		~Buttons();
		
		bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);
		
		Button* pushButton(std::string caption, void (*callback)(Button*, void* lParam), void* lParam, int relX, int relY, bool bistate = false, bool gsmall = false);
		Button* getButton(uint32_t bid);
		
		/* Get amount of buttons */
		uint32_t getAmount();
		
		/* Disable all buttons */
		void enable();
		void disable();
		bool disabled();
		
		void draw(int surface);
	protected:
		bool onMouseMovement(int x, int y, bool leftButtonDown);
		bool onMouseClick(int x, int y);
		bool onMouseRelease(int x, int y);
		
	private:
		std::map<uint32_t, Button*> m_buttons;
		uint32_t m_nextId;
		uint32_t m_activeButton;
		
		/* The container can be disabled */
		bool m_disabled;
};

#endif
