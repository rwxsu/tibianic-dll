#ifndef __GUIPARENTH__
#define __GUIPARENTH__

#include <iostream>
#include <vector>
#include <map>
#include <stdint.h>
#include <assert.h>

#include "guielement.h"
#include "label.h"
#include "objectslot.h"
#include "skin.h"
#include "progressbar.h"
#include "checkbox.h"
#include "inputbox.h"

class GUIParent : public GUIElement {
	public:
		enum location_t {
			BUTTONS_1_LOCATION = 0,
			BUTTONS_2_LOCATION,
			
			RESERVED_1_LOCATION,
			RESERVED_2_LOCATION,
			
			/* Locations for any usage */
			CONTROL_1_LOCATION,
			CONTROL_2_LOCATION,
			CONTROL_3_LOCATION,
			CONTROL_4_LOCATION,
			CONTROL_5_LOCATION,
			CONTROL_6_LOCATION,
			CONTROL_7_LOCATION,
			CONTROL_8_LOCATION
		};
		
		GUIParent(int x, int y, int width, int height);	
		GUIParent* getGUIParent(){return this;}
		
		/* Virtual functions to inherit */
		virtual bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);
		virtual bool onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input);
		
		/* Focus element */
		void focus(GUIElement* child);
		
		/* Virtual draw events */
		virtual void draw(int surface);
		
		/* Virtual tick events */
		virtual void tick(uint32_t ticks);
		
		/* Static method to draw all children */
		static void drawAll(GUIParent* parent, int surface);
		
		/* Static method to tick all children */
		static void tickAll(GUIParent* parent, uint32_t ticks);
		
		/* Get controls */
		template<class T> T* getControl(location_t location){
			return (T*)reinterpret_cast<T*>(m_children[m_locations[location]]);
		}
		
		/* Create controls */
		void addScroll(location_t location, int x, int y, Scroll_t type, int min, int max, int length, double size, int step);
		void addMemoBox(location_t location, int x, int y, int width, int visibleLines);
		void addListBox(location_t location, int x, int y, int width, int visibleItems);
		void addCheckBox(location_t location, int x, int y, int width, bool border = true);
		void addButtons(location_t location, int x, int y);
		void addLabel(location_t location, int x, int y);
		void addLabelEx(location_t location, int x, int y, int width);
		void addProgressBar(location_t location, int x, int y, int width, int height, int min, int max, int red, int green, int blue);
		void addInputBox(location_t location, int x, int y, int width);
		void addSkin(location_t location, int x, int y, int width, int height, int skinId);
		
		int addChild(GUIElement* child);
		void removeChild(GUIElement* child);
	protected:
		std::vector<GUIElement*> m_children;
		std::map<location_t, int> m_locations;
		GUIElement* m_focusChild;
};

#endif
