#include "buttons.h"
#include "functions.h"

Button::Button(std::string caption, void (*function)(Button*, void*), void* lParam, int x, int y, bool bistate /*= false*/, bool gsmall /*= false*/){
	m_active = false;
	m_enabled = true;
	m_text = true;
	
	if(gsmall){
		setGraphics(12, 43, BUTTON_HEIGHT);
	} else {
		setGraphics(18, BUTTON_WIDTH, BUTTON_HEIGHT);
	}
	
	m_position.x = x;
	m_position.y = y;
	
	m_callback = function;
	m_callbackParam = lParam;
	
	m_caption = caption;
	m_bistate = bistate;
}

void Button::setCaption(std::string caption){
	m_caption = caption;
}

void Button::setGraphics(int id, int width, int height){
	m_skin = id;
	m_width = width;
	m_height = height;
}

void Button::draw(int nSurface, int x, int y){
	Painter::drawSkin(nSurface, x + m_position.x, y + m_position.y, m_width, m_height, m_skin + m_active, 0, 0);
	
	if(m_text){
		if(!m_enabled){
			Painter::drawText(nSurface, x + m_position.x + (m_width >> 1) + m_active, y + m_position.y + (m_height >> 1) - 3 + m_active, 3, 162, 162, 162, (char*)m_caption.c_str(), 1);
		} else {
			Painter::drawText(nSurface, x + m_position.x + (m_width >> 1) + m_active, y + m_position.y + (m_height >> 1) - 3 + m_active, 3, 255, 255, 255, (char*)m_caption.c_str(), 1);
		}
	}
}

bool Button::click(){
	if(m_callback){
		m_callback(this, m_callbackParam);
	}
	
	return true;
}

void Button::enable(){
	m_enabled = true;
}

void Button::disable(){
	m_enabled = false;
}

bool Button::enabled(){
	return m_enabled;
}

void Button::activate(){
	m_active = true;
}

void Button::deactivate(){
	m_active = false;
}

bool Button::active(){
	return m_active;
}

int Button::getGraphics(){
	return m_skin;
}

void Button::setActive(bool active){
	m_active = active; 
}

void Button::setPrintable(bool printable){
	m_text = printable; 
}

void Button::setPosition(int x, int y){
	m_position.x = x;
	m_position.y = y;
}

/* Buttons */

Buttons::Buttons(int x, int y) : GUIElement(x, y, 0, 0) {
	m_nextId = 1;
	m_activeButton = 0;
	m_disabled = false;
	
	m_position.x = x;
	m_position.y = y;
}

Buttons::~Buttons(){
	for(std::map<uint32_t, Button*>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it){
		delete it->second;
	}
}

Button* Buttons::pushButton(std::string caption, void (*callback)(Button*, void* lParam), void* lParam, int x, int y, bool bistate /*= false*/, bool gsmall /*= false*/){
	Button* button = new Button(caption, callback, lParam, x, y, bistate, gsmall);
	m_buttons[m_nextId++] = button;
	
	return button;
}

Button* Buttons::getButton(uint32_t bid){
	std::map<uint32_t, Button*>::iterator it = m_buttons.find(bid);
	if(it != m_buttons.end()){
		return it->second;
	}
	
	return NULL;
}

uint32_t Buttons::getAmount(){
	return m_buttons.size();
}

void Buttons::enable(){
	m_disabled = false;
}

void Buttons::disable(){
	m_disabled = true;
}

bool Buttons::disabled(){
	return m_disabled;
}

void Buttons::draw(int surface){
	/* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
	
	for(std::map<uint32_t, Button*>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it){
		it->second->draw(surface, absolute->x, absolute->y);
	}
}

bool Buttons::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	if(!m_disabled){
		switch(event){
			case EVENT_MOVEMENT: {
				return onMouseMovement(x, y, leftButtonDown);
			}
			
			case EVENT_BUTTON: {
				if(leftButtonDown){
					return onMouseClick(x, y);
				}
				
				return onMouseRelease(x, y);
			}
		}
	}
	
	return false;
}

bool Buttons::onMouseMovement(int x, int y, bool leftButtonDown){
	if(leftButtonDown && m_activeButton != 0){
		m_buttons[m_activeButton]->m_active = false;
	
		for(std::map<uint32_t, Button*>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it){
			Button* button = it->second;
			if(x >= m_position.x + button->m_position.x && x <= m_position.x + button->m_width + button->m_position.x && y >= m_position.y + button->m_position.y && y <= m_position.y + button->m_height + button->m_position.y){
				if(m_activeButton == it->first){
					button->m_active = true;
					return true;
				}
			}
		}
	}
	
	return false;
}

bool Buttons::onMouseClick(int x, int y){
	for(std::map<uint32_t, Button*>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it){
		Button* button = it->second;
		if(!button->m_enabled){
			continue;
		}
		
		if(x >= m_position.x + button->m_position.x && x <= m_position.x + button->m_width + button->m_position.x && y >= m_position.y + button->m_position.y && y <= m_position.y + button->m_height + button->m_position.y){
			if(button->m_bistate){
				button->m_active = !button->m_active;
				button->click();
				return true;
			} else {
				button->m_active = true;
				m_activeButton = it->first;
				return true;
			}
		}
	}
	
	return false;
}

bool Buttons::onMouseRelease(int x, int y){
	for(std::map<uint32_t, Button*>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it){
		Button* button = it->second;
		if(x >= m_position.x + button->m_position.x && x <= m_position.x + button->m_width + button->m_position.x && y >= m_position.y + button->m_position.y && y <= m_position.y + button->m_height + button->m_position.y){
			if(m_activeButton == it->first){
				if(!button->m_bistate){
					button->m_active = false;
					button->click();
					m_activeButton = 0;
					return true;
				}
			}
		}
	}
	
	m_activeButton = 0;
	return false;
}
