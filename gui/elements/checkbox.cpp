#include "checkbox.h"

CheckBox::CheckBox(int x, int y, int width, bool border/* = true*/) : GUIElement(x, y, width, 22) {	
	m_border = border;
	m_checked = false;
}

bool CheckBox::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	if(isButtonClickEvent(event, leftButtonDown)){
		if(m_border){
			if(isPointInRectangle(x, y, m_position.x, m_position.y, m_width + 2, 22)){
				m_checked = !m_checked;
				
				/* Notify listeners */
				GUIElement::notify(GUIElement::OnChange);
				
				return true;
			}
		} else {
			if(isPointInRectangle(x, y, m_position.x, m_position.y, Tibia::TextMetric(FONT_BIG, (char*)m_text.c_str(), m_text.length()), 12)){
				m_checked = !m_checked;
				
				/* Notify listeners */
				GUIElement::notify(GUIElement::OnChange);
				
				return true;
			}
		}
	}
	
	return false;
}

void CheckBox::setText(std::string text){
	m_text = text;
}

bool CheckBox::checked(){
	return m_checked;
}

void CheckBox::check(){
	m_checked = true;
}

void CheckBox::uncheck(){
	m_checked = false;
}

void CheckBox::draw(int surface){
	/* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
	
	if(m_border){
		Painter::drawBorders(surface, absolute->x, absolute->y, m_width + 2, 22);
		Painter::drawSkin(surface, absolute->x + 5, absolute->y + 4, 12, 12, m_checked ? 150 : 151, 0, 0);
		Painter::drawText(surface, absolute->x + 5 + 18, absolute->y + 4 + 1, FONT_BIG, 175, 175, 175, (char*)m_text.c_str(), 0);
	} else {
		Painter::drawSkin(surface, absolute->x, absolute->y, 12, 12, m_checked ? 150 : 151, 0, 0);
		Painter::drawText(surface, absolute->x + 18, absolute->y + 1, FONT_BIG, 175, 175, 175, (char*)m_text.c_str(), 0);
	}
}
