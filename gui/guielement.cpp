#include "guielement.h"
#include "guiparent.h"

GUIElement::GUIElement(int x, int y, int width, int height){
	m_position.x = x;
	m_position.y = y;
	
	m_width = width;
	m_height = height;
	
	m_active = true;
	m_hidden = false;
  m_focusable = false;
  m_focused = false;
	
	m_parent = NULL;
}

void GUIElement::addListener(GUIElement::Events event, Listener listener){
	m_listeners[event].push_back(listener);
}

void GUIElement::notify(GUIElement::Events event){
	std::map< GUIElement::Events, std::vector<Listener> >::iterator type = m_listeners.find(event);
	
	if(type != m_listeners.end()){
		for(std::vector<Listener>::iterator it = type->second.begin(); it != type->second.end(); ++it){
			(*it)(this);
		}
	}
}

bool GUIElement::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	Position* relative = GUIElement::getRelativePos();
	
	/* Notify listeners */
	if(isPointInRectangle(x, y, relative->x, relative->y, GUIElement::getWidth(), GUIElement::getHeight())){
		switch(event){
			case EVENT_BUTTON: {
				GUIElement::notify(GUIElement::OnClick);
				break;
			}
			
			case EVENT_DOUBLE_CLICK: {
				GUIElement::notify(GUIElement::OnDoubleClick);
				break;
			}
			
			case EVENT_MOVEMENT: {
				GUIElement::notify(GUIElement::OnMouseOver);
				break;
			}
			
			default: break;
		}
	}
	
	/* Nothing happened in general */
	return false;
}

bool GUIElement::onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input){
	/* Notify listeners */
	GUIElement::notify(GUIElement::OnKeyboard);
	
	/* Nothing happened in general */
	return false;
}

void GUIElement::draw(int surface){

}

void GUIElement::tick(uint32_t ticks){
	
}

void GUIElement::setPosition(int x, int y){
	m_position = Position(x, y, 0);
}

void GUIElement::setWidth(int width){
	m_width = width;
}

void GUIElement::setHeight(int height){
	m_height = height;
}

Position* GUIElement::getRelativePos(){
	static Position position;
	
	/* Set current position */
	position.x = m_position.x;
	position.y = m_position.y;
	
	return &position;
}

Position* GUIElement::getAbsolutePos(){
	static Position position;
	
	/* Set current position */
	position.x = 0;
	position.y = 0;
	
	/* Get parent position */
	GUIElement* parent = m_parent;
	while(parent){
		position.x = position.x + parent->m_position.x;
		position.y = position.y + parent->m_position.y;
		
		parent = parent->getParent();
	}
	
	/* Add relative position */
	position.x = position.x + m_position.x;
	position.y = position.y + m_position.y;
	
	return &position;
}

int GUIElement::getWidth(){
	return m_width;
}

int GUIElement::getHeight(){
	return m_height;
}

bool GUIElement::active(){
	return m_active;
}

void GUIElement::activate(){
	m_active = true;
}

void GUIElement::deactivate(){
	m_active = false;
}

bool GUIElement::hidden(){
	return m_hidden;
}

void GUIElement::hide(){
	m_hidden = true;
}

void GUIElement::show(){
	m_hidden = false;
}

bool GUIElement::focusable(){
  return m_focusable;
}

void GUIElement::focusable(bool focusable){
  m_focusable = focusable;
}

bool GUIElement::focus(){
  return m_focused;
}

void GUIElement::focus(bool focused){
  m_focused = focused;
}

void GUIElement::setParent(GUIParent* parent){
	m_parent = parent;
}

GUIParent* GUIElement::getParent(){
  return m_parent;
}
