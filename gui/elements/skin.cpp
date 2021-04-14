#include "skin.h"

Skin::Skin(int x, int y, int width, int height, int skinId) : GUIElement(x, y, width, height) {
	m_skinId = skinId;
}

bool Skin::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	if(isButtonClickEvent(event, leftButtonDown)){
		if(isPointInRectangle(x, y, m_position.x, m_position.y, m_width, m_height)){
			return true;
		}
	}
	
	return false;
}

void Skin::setSkinId(int skinId){
	m_skinId = skinId;
}

void Skin::draw(int surface){
	/* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
	
	Painter::drawSkin(surface, absolute->x, absolute->y, m_width, m_height, m_skinId, 0, 0);
}
