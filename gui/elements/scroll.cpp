#include "scroll.h"
#include "functions.h"

void Scroll::scrollUp(Button* button, void* lParam){
	Scroll* scroll = reinterpret_cast<Scroll*>(lParam);
	
	scroll->m_value = std::max(scroll->m_min, scroll->m_value - scroll->m_step);
	scroll->update();
}

void Scroll::scrollDown(Button* button, void* lParam){
	Scroll* scroll = reinterpret_cast<Scroll*>(lParam);
	
	scroll->m_value = std::min(scroll->m_max, scroll->m_value + scroll->m_step);
	scroll->update();
}

Scroll::Scroll(int x, int y, Scroll_t type, int min, int max, int length, double size, int step) : GUIParent(x, y, 0, 0) {
	m_type = type;	
	m_min = min;
	m_max = max;
	m_length = length;	

	setSize(size);
	
	m_value = m_min;
	m_click = 0;
	m_hold = false;
	
	m_step = step;	
	
	/* Add buttons as child */
	GUIParent::addButtons(CONTROL_1_LOCATION, 0, 0);
	
	if(type == SCROLL_VERTICAL){
		m_upButton = GUIParent::getControl<Buttons>(CONTROL_1_LOCATION)->pushButton("up", &Scroll::scrollUp, this, 0, 0);
		m_upButton->setGraphics(157, 12, 12);
		
		m_downButton = GUIParent::getControl<Buttons>(CONTROL_1_LOCATION)->pushButton("down", &Scroll::scrollDown, this, 0, length - SCROLL_ICON_DIMENSION);
		m_downButton->setGraphics(159, 12, 12);
	} else {
		m_upButton = GUIParent::getControl<Buttons>(CONTROL_1_LOCATION)->pushButton("up", &Scroll::scrollUp, this, 0, 0);
		m_upButton->setGraphics(165, 12, 12);	
		
		m_downButton = GUIParent::getControl<Buttons>(CONTROL_1_LOCATION)->pushButton("down", &Scroll::scrollDown, this, length - SCROLL_ICON_DIMENSION, 0);
		m_downButton->setGraphics(167, 12, 12);
	}
	
	m_upButton->setPrintable(false);
	m_downButton->setPrintable(false);
	
	/* Lets update cache */
	Scroll::update();
}

bool Scroll::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	if(GUIParent::onMouseEvent(event, x, y, leftButtonDown, rightButtonDown)){
		return true;
	}
	
	switch(event){
		case EVENT_SCROLL_DOWN: {
			Scroll::scrollDown(m_downButton, this);
			return true;
		}
		
		case EVENT_SCROLL_UP: {
			Scroll::scrollUp(m_upButton, this);
			return true;
		}
		
		default: break;
	}
	
	if(leftButtonDown && event == EVENT_MOVEMENT){
		if(m_hold){	
			int displacement;		
			if(m_type == SCROLL_VERTICAL){
				displacement = y - m_click;
			} else {
				displacement = x - m_click;
			}
			
			m_value = (float)((float)displacement / (float)(m_length - SCROLL_ICON_DIMENSION * 2 - m_mark)) * (float)(m_max - m_min); 			
			m_value = std::min(m_max, std::max(m_min, m_value + m_min));
			
			/* Lets update cache */
			Scroll::update();
	
			return true;
		}
	}
	
	// We are not dragging scroll anymore
	m_hold = false;
	
	if(isButtonClickEvent(event, leftButtonDown)){
		if(m_type == SCROLL_VERTICAL){
			if(isPointInRectangle(x, y, m_position.x, m_position.y + SCROLL_ICON_DIMENSION + m_displacement, SCROLL_ICON_DIMENSION, m_mark)){
				m_click = y - m_displacement;
				m_hold = true;
				
				return true;
			} else if(isPointInRectangle(x, y, m_position.x, m_position.y + SCROLL_ICON_DIMENSION, SCROLL_ICON_DIMENSION, m_length - SCROLL_ICON_DIMENSION * 2)){ 
				float value = (float)(y - (m_position.y + SCROLL_ICON_DIMENSION)) / (float)(m_length - SCROLL_ICON_DIMENSION * 2.0f);		
				m_value = m_min + (m_max - m_min) * value;
				
				/* Lets update cache */
				Scroll::update();
	
				return true;
			}
		} else {
			if(isPointInRectangle(x, y, m_position.x + SCROLL_ICON_DIMENSION + m_displacement, m_position.y, m_mark, SCROLL_ICON_DIMENSION)){
				m_click = x - m_displacement;
				m_hold = true;
				
				return true;
			} else if(isPointInRectangle(x, y, m_position.x + SCROLL_ICON_DIMENSION, m_position.y, m_length - SCROLL_ICON_DIMENSION * 2, SCROLL_ICON_DIMENSION)){ 
				float value = (float)(x - (m_position.x + SCROLL_ICON_DIMENSION)) / (float)(m_length - SCROLL_ICON_DIMENSION * 2.0f);		
				m_value = m_min + (m_max - m_min) * value;
				
				/* Lets update cache */
				Scroll::update();
				
				return true;
			}
		}
	}
	
	return false;
}

void Scroll::update(){
	m_mark = std::max(0.0, (m_length - SCROLL_ICON_DIMENSION * 2) * m_size);
	m_displacement = (float)(m_length - SCROLL_ICON_DIMENSION * 2 - m_mark) * ((float)(m_value - m_min) / (float)(m_max - m_min));
}

int Scroll::getPosition(){
	return m_value;
}
		
int Scroll::getMax(){
	return m_max;
}

int Scroll::getMin(){
	return m_min;
}

void Scroll::setMin(int min){
	m_min = min;
}

void Scroll::setMax(int max){
	m_max = max;
}

void Scroll::setSize(double size){
	m_size = std::min(1.0, std::max(0.0, size));
	if(m_size == 1.0){
		m_value = m_min;
	}
	
	/* Lets update cache */
	Scroll::update();
}

void Scroll::setLength(int lenValue){
	m_length = lenValue;
	
	// update down button (the one on the edge) since we only increased or decreased length
	if(m_type == SCROLL_VERTICAL){
		m_downButton->setPosition(0, m_length - SCROLL_ICON_DIMENSION);
	} else {
		m_downButton->setPosition(m_length - SCROLL_ICON_DIMENSION, 0);
	}
	
	update();
}

void Scroll::setPosition(int position){
	m_value = std::min(m_max, std::max(m_min, position));
	
	/* Lets update cache */
	Scroll::update();
}

void Scroll::draw(int surface){
	/* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
	
	if(m_type == SCROLL_VERTICAL){
		Painter::drawSkin(surface, absolute->x, absolute->y + SCROLL_ICON_DIMENSION, SCROLL_ICON_DIMENSION, m_length - SCROLL_ICON_DIMENSION * 2, 153, 0, 0);
	} else {
		Painter::drawSkin(surface, absolute->x + SCROLL_ICON_DIMENSION, absolute->y, m_length - SCROLL_ICON_DIMENSION * 2, SCROLL_ICON_DIMENSION, 161, 0, 0);
	}
	
	if(m_size != 1.0){
		if(m_type == SCROLL_VERTICAL){
			Painter::drawSkin(surface, absolute->x, absolute->y + SCROLL_ICON_DIMENSION + m_displacement, SCROLL_ICON_DIMENSION, 6, 154, 0, 0);
			if(m_mark){
				Painter::drawSkin(surface, absolute->x, absolute->y + SCROLL_ICON_DIMENSION + m_displacement + 6, SCROLL_ICON_DIMENSION, m_mark - 12, 155, 0, 0);
			}
			Painter::drawSkin(surface, absolute->x, absolute->y + SCROLL_ICON_DIMENSION + m_displacement + m_mark - 6, SCROLL_ICON_DIMENSION, 6, 156, 0, 0);
		} else {
			Painter::drawSkin(surface, absolute->x + SCROLL_ICON_DIMENSION + m_displacement, absolute->y, 6, SCROLL_ICON_DIMENSION, 162, 0, 0);
			if(m_mark){
				Painter::drawSkin(surface, absolute->x + SCROLL_ICON_DIMENSION + m_displacement + 6, absolute->y, m_mark - 12, SCROLL_ICON_DIMENSION, 163, 0, 0);
			}
			Painter::drawSkin(surface, absolute->x + SCROLL_ICON_DIMENSION + m_displacement + m_mark - 6, absolute->y, 6, SCROLL_ICON_DIMENSION, 164, 0, 0);
		}
	}
}
