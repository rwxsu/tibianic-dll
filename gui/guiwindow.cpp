#include "guiwindow.h"
#include "gui.h"
#include "tools.h"
#include "main.h"

#include <sstream>
#include <limits.h>

/* Static functions of GUIWindow class */
void GUIWindow::closeWindow(Button* button, void* lpParam){
	GUIWindow* window = reinterpret_cast<GUIWindow*>(lpParam);
	
	/* Release window will delete this object */
	g_gui->releaseWindow(window);
}

void GUIWindow::minimizeWindow(Button* button, void* lpParam){
	GUIWindow* window = reinterpret_cast<GUIWindow*>(lpParam);
	
	/* Minimize will work both ways, as maximize and minimize */
	window->m_minimize = !window->m_minimize;
	
	/* Choose valid icon for the button */
	if(window->m_minimize){
		button->setCaption("O");
	} else {
		button->setCaption("-");
	}
}

/* Normal functions of GUIWindow class */
GUIWindow::GUIWindow(std::string caption, int x, int y, int width, int height, bool minimize, bool close) : GUIParent(x, y, width, height) {
	#define BOTTOM_BUTTONS_X 8
	#define BOTTOM_BUTTONS_Y 6 - BUTTON_HEIGHT

	/* Default values */
	m_minimize = false;
	
	/* Buttons controls */
	GUIParent::addButtons(GUIParent::BUTTONS_1_LOCATION, GUIElement::getWidth() - BOTTOM_BUTTONS_X, GUIElement::getHeight() - BOTTOM_BUTTONS_Y);
	GUIParent::addButtons(GUIParent::BUTTONS_2_LOCATION, 0, 0);
	
	/* Label with title */
	GUIParent::addLabel(GUIParent::RESERVED_1_LOCATION, GUIElement::getWidth() >> 1, 5);
	
	/* Girder */
	//GUIParent::addSkin(GUIParent::RESERVED_2_LOCATION, 13, GUIElement::getHeight() - 40, GUIElement::getWidth() - 13 * 2, 2, 41);
	
	/* Set caption text */
	GUIParent::getControl<Label>(GUIParent::RESERVED_1_LOCATION)->setText(Label_t(FONT_BIG, 143, 143, 143, 1), caption);
	
	/* Icons of the window */
	Buttons* icons = GUIParent::getControl<Buttons>(GUIParent::BUTTONS_2_LOCATION);
	
	if(close){
		icons->pushButton("X", &GUIWindow::closeWindow, this, 3 + GUIElement::getWidth() - 16 * (icons->getAmount() + 1), 1, false, true)->setGraphics(24, 16, 16);
	}
	
	if(minimize){
		icons->pushButton("-", &GUIWindow::minimizeWindow, this, 3 + GUIElement::getWidth() - 16 * (icons->getAmount() + 1), 1, false, true)->setGraphics(24, 16, 16);
	}
}

void GUIWindow::onResolution(){
	static Position position;
	
	/* TODO - only dialog should be centered */
	
	/* Lets update position to the center since resolution has changed */
	position.x = std::max((int32_t)0, (int32_t)(g_dll.m_screenWidth / 2 - GUIElement::getWidth() / 2));
	position.y = std::max((int32_t)0, (int32_t)(g_dll.m_screenHeight / 2 - GUIElement::getHeight() / 2));
	
	/* Set the position of the element */
	GUIElement::setPosition(position.x, position.y);
}

bool GUIWindow::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	/* Absoute position of this window */
	Position* absolute = GUIElement::getAbsolutePos();
	
	/* If window is minimized only icons buttons can be iteracted with */
	if(m_minimize){
		return GUIParent::getControl<Buttons>(GUIParent::BUTTONS_2_LOCATION)->onMouseEvent(event, x - absolute->x, y - absolute->y, leftButtonDown, rightButtonDown);
	}
	
	/* Dispatch the event among children */
	return GUIParent::onMouseEvent(event, x, y, leftButtonDown, rightButtonDown);
}

bool GUIWindow::onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input){
	/* If window is minimized only icons buttons can be iteracted with */
	if(m_minimize){
		return GUIParent::getControl<Buttons>(GUIParent::BUTTONS_2_LOCATION)->onKeyboardEvent(event, state, input);
	}
	
	/* Lets check some basic actions before dispatching event to children */
	if(event == EVENT_KEY_DOWN){
		switch(state){
			case KEYBOARD_STATE_ESCAPE:
			case KEYBOARD_STATE_ENTER: {
				Buttons* buttons = getControl<Buttons>(GUIParent::BUTTONS_1_LOCATION);
				if(buttons->getAmount()){
					uint32_t buttonId = buttons->getAmount();
					
					/* The escape event should always choose the first button (which should be cancel button) */
					if(state == KEYBOARD_STATE_ESCAPE){
						buttonId = 1;
					}
					
					if(Button* button = buttons->getButton(buttonId)){
						if(button->enabled()){
							return button->click();
						}
					}
				}
				
				break;
			}
			
			default: break;
		}
	}
	
	// Dispatch the event among children
	return GUIParent::onKeyboardEvent(event, state, input);
}

Button* GUIWindow::pushButton(std::string caption, void (*callback)(Button*, void* lParam), void* param, int x, int y, bool bistate /*= false*/, bool mini /*= false*/){
	return getControl<Buttons>(GUIParent::BUTTONS_1_LOCATION)->pushButton(caption, callback, param, x, y, bistate, mini);
}

int32_t GUIWindow::getButtonNextX(){
	Buttons* buttons = getControl<Buttons>(GUIParent::BUTTONS_1_LOCATION);
	return -1 * ((buttons->getAmount() + 1) * BUTTON_WIDTH + buttons->getAmount() * 10);
}

int32_t GUIWindow::getButtonNextY(){
	return 0;
}

void GUIWindow::draw(int surface){
	/* Absoute position of this window */
	Position* absolute = GUIElement::getAbsolutePos();
	
	if(m_minimize){
		Painter::drawSkin(surface, absolute->x + 2, absolute->y, m_width, 18, 7, 0, 0);		
		Painter::drawSkin(surface, absolute->x + 1, absolute->y, m_width + 2, 18 - 1, 49, 0, 0);
		
		Painter::drawBorders(surface, absolute->x, absolute->y, m_width + 4, 18 - 1);
		
		/* Lets draw small buttons to manipulate window */
		getControl<Buttons>(GUIParent::BUTTONS_2_LOCATION)->draw(surface);
		
		/* Lets draw caption above header of the window */
		getControl<Label>(GUIParent::RESERVED_1_LOCATION)->draw(surface);
	} else {
		Painter::drawSkin(surface, absolute->x + 2, absolute->y, m_width, m_height, 7, 0, 0);		
		Painter::drawSkin(surface, absolute->x, absolute->y, 4, m_height, 47, 0, 0); // left
		Painter::drawSkin(surface, absolute->x + m_width + 1, absolute->y, 4, m_height + 4, 48, 0, 0); // right
		Painter::drawSkin(surface, absolute->x + 4, absolute->y + m_height, m_width - 2, 4, 50, 0, 0); // bottom
		Painter::drawSkin(surface, absolute->x + 1, absolute->y, m_width + 2, 17, 49, 0, 0); // top	
		Painter::drawSkin(surface, absolute->x, absolute->y + m_height, 4, 4, 45, 0, 0); // left/bottom
		Painter::drawSkin(surface, absolute->x + m_width + 1, absolute->y + m_height, 4, 4, 46, 0, 0); // right/bottom
		
		/* GUIParent class will take care of every child */
		GUIParent::draw(surface);
		
		/* In case a window requires extra function for drawing */
		drawSelf(surface);
	}
}
