#ifndef __GUIELEMENTH__
#define __GUIELEMENTH__

#include <iostream>
#include <stdint.h>
#include <vector>
#include <map>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "functions.h"
#include "position.h"
#include "painter.h"

class GUIElement;
class GUIParent;

class GUIElement {
	friend class GUIParent;
	
	/* Definitions of variables */
	public:
		enum Events {
			OnClick,
			OnDoubleClick,
			OnMouseOver,
			OnKeyboard,
			OnChange
		};
		
		typedef boost::function<void (GUIElement* caller)> Listener;
	
	/* Class functions */
	public:
		GUIElement(int x, int y, int width, int height);
		
		/* Listeners & events */
		void addListener(GUIElement::Events event, Listener listener);
		void notify(GUIElement::Events event);
		
		/* Virtual functions to inherit */
		virtual bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);
		virtual bool onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input);
		
		/* Virtual functions for GUIParent */
		virtual GUIParent* getGUIParent(){return NULL;}
		
		/* Create element */
		template<class T> static GUIElement* createElement(GUIParent* parent = NULL){
			GUIElement* element = new T();
			if(parent){
				//parent->addChild(element);
			}
			
			return element;
		}
		
		/* Draw events */
		virtual void draw(int surface);
		
		/* Tick events */
		virtual void tick(uint32_t ticks);
		
		/* Set current relative position of the element */
		void setPosition(int x, int y);
		
		/* Set width and height of the element */
		virtual void setWidth(int width);
		virtual void setHeight(int height);
		
		/* Get current position of the element */
		Position* getRelativePos();
		Position* getAbsolutePos();
		
		/* Get current dimension */
		int getWidth();
		int getHeight();
		
		/* Element can be inactive */
		bool active();
		void activate();
		void deactivate();
		
		/* Element can be hidden */
		bool hidden();
		void hide();
		void show();
   
    /* Element can be focusable */
    bool focusable();
    void focusable(bool focusable);
    bool focus();
    void focus(bool focused);
		
		/* Element can have parent */
		void setParent(GUIParent* parent);
    GUIParent* getParent();
	protected:
		Position m_position;
		int m_width;
		int m_height;
		
		bool m_active;
		bool m_hidden;
    bool m_focusable;
    bool m_focused;
		
		GUIParent* m_parent;
		
		/* Listeners */
		std::map< GUIElement::Events, std::vector<Listener> > m_listeners;
};

#endif
