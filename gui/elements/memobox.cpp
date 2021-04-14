#include "memobox.h"
#include "tools.h"

#define MEMOBOX_FONT_HEIGHT 12

MemoBox::MemoBox(int x, int y, int width, int visibleLines) : GUIParent(x, y, width, visibleLines * 12) {
  m_visibleLines = visibleLines;
  
  /* Scroll to navigate through the list */
	GUIParent::addScroll(CONTROL_1_LOCATION, width - MEMOBOX_FONT_HEIGHT, 0, SCROLL_VERTICAL, 0, 0, visibleLines * MEMOBOX_FONT_HEIGHT, 1.0, 1);
  
  /* This kind of element can acquire focus */
  GUIElement::focusable(true);
}

bool MemoBox::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	if(event == EVENT_SCROLL_DOWN || event == EVENT_SCROLL_UP){
		if(!isPointInRectangle(x, y, m_position.x, m_position.y, m_width, m_visibleLines * 12)){
			return false;
		}
	}
	
	// Dispatch the event among children
	return GUIParent::onMouseEvent(event, x, y, leftButtonDown, rightButtonDown);
}

bool MemoBox::onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input){
	if(event == EVENT_KEY_DOWN){
		switch(state){
			case KEYBOARD_STATE_UP: {
				getControl<Scroll>(CONTROL_1_LOCATION)->setPosition(std::max(0, getControl<Scroll>(CONTROL_1_LOCATION)->getPosition() - 1));
				return true;
			}
			
			case KEYBOARD_STATE_DOWN: {
				getControl<Scroll>(CONTROL_1_LOCATION)->setPosition(std::min((int)m_lines.size() - 1, getControl<Scroll>(CONTROL_1_LOCATION)->getPosition() + 1));	
				return true;
			}
			
			default: break;
		}
	}
	
	// Dispatch the event among children
	return GUIParent::onKeyboardEvent(event, state, input);
}

void MemoBox::setText(std::string text){
	m_lines = wrapTextLines(1, text, m_width - 25);
	
	int hiddenLines = std::max(0, (int)m_lines.size() - m_visibleLines);
	
	getControl<Scroll>(CONTROL_1_LOCATION)->setMax(hiddenLines);
	getControl<Scroll>(CONTROL_1_LOCATION)->setPosition(0);
	
	if(hiddenLines){
		getControl<Scroll>(CONTROL_1_LOCATION)->setSize(std::max(0.2, (double)m_visibleLines / (double)(hiddenLines + m_visibleLines)));
	} else {
		getControl<Scroll>(CONTROL_1_LOCATION)->setSize(1.0);
	}
}

void MemoBox::draw(int nSurface){
	/* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
	
	Painter::drawRectangle(nSurface, absolute->x, absolute->y, m_width, MEMOBOX_FONT_HEIGHT * m_visibleLines, 64, 64, 64);
	Painter::drawBorders(nSurface, absolute->x - 1, absolute->y - 1, m_width + 2, m_visibleLines * MEMOBOX_FONT_HEIGHT + 1);
	
	for(int i = getControl<Scroll>(CONTROL_1_LOCATION)->getPosition(), j = 0; i != getControl<Scroll>(CONTROL_1_LOCATION)->getPosition() + m_visibleLines; ++i, ++j){
		if(i >= m_lines.size()){
			break;
		}

		Painter::drawText(nSurface, absolute->x + 2, absolute->y + 2 + j * MEMOBOX_FONT_HEIGHT, FONT_BIG, 175, 175, 175, (char*)m_lines[i].c_str(), 0);
	}
}