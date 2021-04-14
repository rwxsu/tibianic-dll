#include "inputbox.h"

#define INPUT_BOX_BACK_X 1
#define INPUT_BOX_BACK_Y 1

#define INPUT_BOX_TEXT_X 3
#define INPUT_BOX_TEXT_Y 3

InputBox::InputBox(int x, int y, int width) : GUIElement(x, y, width + INPUT_BOX_BACK_X * 2, 14 + INPUT_BOX_BACK_Y * 2){
	/* Cache related variables */
	m_cacheLength = 0;
	m_cacheLocation = 0;
	m_cacheCursor = 0;
	m_cacheSelection = 0;
	
	/* There isn't any position yet which was clicked on */
	m_cacheClick = -1;
	
	/* Text is not selected */
	m_textSelected = false;
	
	/* Cursor related variables */
	m_increaseCursor = false;
	m_decreaseCursor = false;
  
  /* This kind of element can acquire focus */
  GUIElement::focusable(true);
}

bool InputBox::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	/* Relative position of this element */
	Position* relative = GUIElement::getRelativePos();
	
	if(isButtonClickEvent(event, leftButtonDown)){
		if(isPointInRectangle(x, y, relative->x, relative->y, GUIElement::getWidth(), GUIElement::getHeight())){
			/* Perhaps we need to change the cursor position? */
			if(isPointInRectangle(x, y, relative->x + INPUT_BOX_TEXT_X, relative->y + INPUT_BOX_TEXT_X, GUIElement::getWidth() - INPUT_BOX_TEXT_Y, GUIElement::getHeight() - INPUT_BOX_TEXT_Y)){
				int position = letter(x - relative->x - INPUT_BOX_TEXT_X);
				
				m_cacheCursor = position;
				m_cacheClick = position;
			}
			
			/* Set the selection to none */
			m_cacheSelection = 0;
			
			/* Set the selection of whole text to false */
			m_textSelected = false;
		}
	}
	
	if(isMouseMoveEvent(event, leftButtonDown)){
		if(isPointInRectangle(x, y, relative->x + INPUT_BOX_TEXT_X, relative->y + INPUT_BOX_TEXT_X, GUIElement::getWidth() - INPUT_BOX_TEXT_Y, GUIElement::getHeight() - INPUT_BOX_TEXT_Y)){
			int position = letter(x - relative->x - INPUT_BOX_TEXT_X);
			
			// The new cursor position needs to be set
			m_cacheCursor = position;
			
			if(m_cacheClick != -1){ // It can happen when we click outside INPUT_BOX_TEXT_X / INPUT_BOX_TEXT_Y arena
				m_cacheSelection = m_cacheClick - m_cacheCursor;
			}
		}
	}
	
	if(isButtonReleaseEvent(event, leftButtonDown)){
		m_cacheClick = -1;
	}
	
	return false;
}

bool InputBox::onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input){
	if(GUIElement::active()){
		if(event == EVENT_KEY_DOWN){
			switch(state){
				case KEYBOARD_STATE_PASTE: {
					return InputBox::action(InputBox::Paste_Text);
				}
				
				case KEYBOARD_STATE_COPY: {
					return InputBox::action(InputBox::Copy_Text);
				}
				
				case KEYBOARD_STATE_SELECTALL: {
					return InputBox::action(InputBox::Select_Text);
				}
				
				case KEYBOARD_STATE_CUT: {
					return InputBox::action(InputBox::Cut_Text);
				}
				
				case KEYBOARD_STATE_BACK: {
					return InputBox::action(InputBox::Erase_Left);
				}
				
				case KEYBOARD_STATE_DELETE: {
					return InputBox::action(InputBox::Erase_Right);
				}
				
				case KEYBOARD_STATE_LEFT: {
					return InputBox::action(InputBox::Move_Left);
				}
				
				case KEYBOARD_STATE_RIGHT: {
					return InputBox::action(InputBox::Move_Right);
				}
				
				case KEYBOARD_STATE_ASCII: {
					return InputBox::action(InputBox::Insert_Char, input);
				}
				
				default: break;
			}
		}
	}
	
	return false;
}

std::string InputBox::getText(){
	return m_text;
}

int32_t InputBox::letter(int32_t sx){
	int cursor = m_cacheText.length();
	
	for(int i = 1; i <= m_cacheText.length(); i++){
		if(getTotalMetric(FONT_BIG, m_cacheText.substr(0, i)) >= sx){
			cursor = i - 1;
			break;
		}
	}
	
	return cursor;
}

void InputBox::cache(){
	if(m_decreaseCursor){
		if(m_cacheCursor != 0){
			m_cacheCursor = m_cacheCursor - 1;
		} else {
			if(m_cacheLocation != 0){ // else we do nothing
				m_cacheLocation = m_cacheLocation - 1;
			}
		}
		
		// Create cache
		cache_forwards();
		
		// Cursor has been decreased
		m_decreaseCursor = false;
	}
	
	if(m_increaseCursor){
		if(m_cacheCursor != m_cacheLength){
			m_cacheCursor = m_cacheCursor + 1;
		} else {
			if(m_cacheLocation + m_cacheCursor != m_text.length()){ // else we do nothing
				if(m_cacheCursor >= m_cacheText.length() + 1){
					// Nothing
				} else {
					m_cacheCursor++;
				}
			}
		}
		
		// Create cache
		cache_forwards();
		
		if(m_cacheCursor > m_cacheText.length()){
			int cacheLength = m_cacheLength;		
			while(true){ // until we reach the next letter - disadvantage of forward caching
				m_cacheLocation = m_cacheLocation + 1;
				
				// we need to re-create cache again	
				cache_forwards(true);
				
				// Yeah
				m_cacheCursor = m_cacheLength;
				
				if(cacheLength > m_cacheLength){ // letter was removed, but there was none added (at the right side)
					cacheLength = m_cacheLength;
				} else {
					break;
				}
			}
		}
		
		// Cursor has been increased
		m_increaseCursor = false;
	}
}

void InputBox::cache_forwards(bool recreate){
	std::string cacheText;
	
	for(int i = m_cacheLocation; i < m_text.length(); i++){
		if(getTotalMetric(FONT_BIG, cacheText + m_text[i]) + 2 * INPUT_BOX_TEXT_X >= GUIElement::getWidth()){
			break;
		}
		
		if(recreate){ // STILL TODO, related to backwards caching
			if(cacheText.length() == m_cacheLength){
				break;
			}
		}
		
		cacheText.insert(cacheText.end(), m_text[i]);
	}
	
	// Replace cached text
	m_cacheText = cacheText;
	m_cacheLength = cacheText.length();
}

bool InputBox::action(action_t action, int param/* = 0*/){
	switch(action){
		case InputBox::Paste_Text: {
			std::string clipboard = getClipboardText();
			for(size_t i = 0; i < clipboard.length(); i++){
				InputBox::action(InputBox::Insert_Char, clipboard[i]);
			}
			
			break;
		}
		
		case InputBox::Copy_Text: {
			if(m_cacheSelection){
				std::string clipboard;
				
				/* Acquire text to copy based on selection position */
				if(m_textSelected){
					clipboard = m_text;
				} else if(m_cacheSelection < 0){
					clipboard = std::string(m_text.c_str() + m_cacheLocation + m_cacheCursor + m_cacheSelection, std::abs(m_cacheSelection));
				} else {
					clipboard = std::string(m_text.c_str() + m_cacheLocation + m_cacheCursor, m_cacheSelection);
				}
				
				/* Set text of the clipboard */
				setClipboardText(clipboard);
			}
			
			/* There is no need to re-cache */
			return true;
		}
		
		case InputBox::Select_Text: {
			/* Cursor to the far right side */
			while(m_cacheCursor != m_cacheText.length() || m_cacheText.length() + m_cacheLocation != m_text.length()){
				InputBox::action(InputBox::Move_Right);
			}
			
			/* Select all currently visible text */
			m_cacheSelection = -m_cacheText.length();
			
			/* However, in fact whole text is selected */
			m_textSelected = true;
			
			/* There is no need to re-cache */
			return true;
		}
		
		case InputBox::Cut_Text: {
			if(m_cacheSelection){
				InputBox::action(InputBox::Copy_Text);
				InputBox::action(InputBox::Erase_Left);
			}
			
			/* There is no need to re-cache */
			return true;
		}
		
		case InputBox::Erase_Left: {
			/* Location of the letter to remove */
			int location = std::max(0, m_cacheLocation + m_cacheCursor - 1);
			
			if(m_cacheSelection){
				if(m_textSelected){
					/* Purge text */
					m_text = std::string();
					m_cacheText = std::string();
					
					/* Set locations to 0 */
					m_cacheLocation = 0;
					m_cacheCursor = 0;
					
					/* Nothing is partially selected */
					m_cacheSelection = 0;
					
					/* The whole text is not selected */
					m_textSelected = false;
				} else { /* Partial text is selected */
					int32_t cacheSelection = m_cacheSelection;
					m_cacheSelection = 0;
						
					if(cacheSelection < 0){
						for(int32_t i = 0; i < std::abs(cacheSelection); i++){
							InputBox::action(InputBox::Erase_Left);
						}
						
						/* There is no need to re-cache */
						return true;
					} else {
						/* Move the cursor */
						m_cacheCursor = m_cacheCursor + cacheSelection;
						
						for(int32_t i = 0; i < std::abs(cacheSelection); i++){
							InputBox::action(InputBox::Erase_Left);
						}
						
						/* There is no need to re-cache */
						return true;
					}
				}
			} else {
				/* Is the erase even possible? */
				bool possible = !(m_cacheLocation == 0 && m_cacheCursor == 0);
				
				if(possible){
					/* Erase the letter */
					m_text.erase(location, 1);
					
					/* Move the cursor */
					return InputBox::action(InputBox::Move_Left);
				}
			}
			
			break;
		}
		
		case InputBox::Erase_Right: {
			/* Erase selected text */
			if(m_cacheSelection){
				return InputBox::action(InputBox::Erase_Left);
			}
			
			/* Location of the letter to remove */
			int location = std::max(0, m_cacheLocation + m_cacheCursor);
			
			/* Erase the letter */
			if(location != m_text.size()){
				InputBox::action(InputBox::Move_Right);
				InputBox::action(InputBox::Erase_Left);
			}
			
			/* There is no need to re-cache */
			return true;
		}
		
		case InputBox::Move_Left: {
			/* The cursor will be decreased when the cache will be re-created */
			m_decreaseCursor = true;
			
			/* Disable active selection */
			m_cacheSelection = 0;
			
			/* Set the selection of whole text to false */
			m_textSelected = false;
			
			break;
		}
		
		case InputBox::Move_Right: {
			/* The cursor will be increased when the cache will be re-created */
			m_increaseCursor = true;
			
			/* Disable active selection */
			m_cacheSelection = 0;
			
			/* Set the selection of whole text to false */
			m_textSelected = false;
			
			break;
		}
		
		case InputBox::Insert_Char: {
			/* The limit of 255 characters has to be there */
			if(m_text.length() < 0xFF){
				/* Erase selected text */
				if(m_cacheSelection){
					InputBox::action(InputBox::Erase_Left);
				}
				
				/* Insert new letter */
				m_text.insert(m_cacheLocation + m_cacheCursor, std::string((char*)&param, 1));
				
				/* Increase cursor */
				return InputBox::action(InputBox::Move_Right);
			}
			
			return false;
		}
		
		default: break;
	}
	
	// Re-create cache
	InputBox::cache();
	
	// Everything went ok
	return true;
}

void InputBox::draw(int surface){
	/* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
	
	/* Move to new Box class */
	Painter::drawBorders(surface, absolute->x, absolute->y, m_width, m_height);
	Painter::drawRectangle(surface, absolute->x + INPUT_BOX_BACK_X, absolute->y + INPUT_BOX_BACK_Y, m_width - 2, m_height - 1, 54, 54, 54);
	
	if(!m_cacheText.empty()){
		if(m_cacheSelection){
			if(m_cacheSelection > 0){
				int32_t selection = Tibia::TextMetric(FONT_BIG, (char*)m_cacheText.c_str() + m_cacheCursor, m_cacheSelection);
				int32_t cursor = Tibia::TextMetric(FONT_BIG, (char*)m_cacheText.c_str(), m_cacheCursor);
				
				// Draw the selection to the right side from cursor
				Painter::drawRectangle(surface, absolute->x + INPUT_BOX_TEXT_X + cursor, absolute->y + INPUT_BOX_TEXT_Y, selection, 10, 128, 128, 128);
			} else {
				int32_t selection = Tibia::TextMetric(FONT_BIG, (char*)m_cacheText.c_str() + m_cacheCursor + m_cacheSelection, std::abs(m_cacheSelection));
				int32_t cursor = Tibia::TextMetric(FONT_BIG, (char*)m_cacheText.c_str(), m_cacheCursor);
				
				// Draw the selection to the left side from cursor
				Painter::drawRectangle(surface, absolute->x + INPUT_BOX_TEXT_X + cursor - selection, absolute->y + INPUT_BOX_TEXT_Y, selection, 10, 128, 128, 128);
			}
		}
		
		/* Print currently visible cached text */
		Painter::drawText(surface, absolute->x + INPUT_BOX_TEXT_X, absolute->y + INPUT_BOX_TEXT_Y, FONT_BIG, 223, 223, 223, (char*)m_cacheText.c_str(), 0);
	}
	
	/* Print cursor itself if the control is focused */
	if(GUIElement::focus()){
		if(Tibia::GetPlayerInfo(PLAYER_INFO_TICKS) % 10 <= 5){
			Painter::drawRectangle(surface, absolute->x + INPUT_BOX_TEXT_X + Tibia::TextMetric(FONT_BIG, (char*)m_cacheText.c_str(), m_cacheCursor), absolute->y + INPUT_BOX_TEXT_Y, 1, 10, 255, 255, 255);
		}
	}
}
