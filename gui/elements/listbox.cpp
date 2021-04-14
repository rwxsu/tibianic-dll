#include "listbox.h"

ListBox::ListBox(int x, int y, int width, int visibleItems) : GUIParent(x, y, width, visibleItems * 12) {
  m_selectedItem = 0;
	m_visibleItems = visibleItems;
  
  /* Scroll to navigate through the list */
	GUIParent::addScroll(GUIParent::CONTROL_1_LOCATION, width - 12, 0, SCROLL_VERTICAL, 0, 0, visibleItems * 12, 1.0, 1);
  
  /* This kind of element can acquire focus */
  GUIElement::focusable(true);
}

ListBox::~ListBox(){

}

bool ListBox::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	if(event == EVENT_SCROLL_DOWN || event == EVENT_SCROLL_UP){
		if(!isPointInRectangle(x, y, m_position.x, m_position.y, m_width, m_visibleItems * 12)){
			return false;
		}
	}
	
	// Dispatch the event among children
	if(GUIParent::onMouseEvent(event, x, y, leftButtonDown, rightButtonDown)){
		return true;
	}
	
	Scroll* scroll = getControl<Scroll>(GUIParent::CONTROL_1_LOCATION);
	if(isButtonClickEvent(event, leftButtonDown)){
		for(int i = scroll->getPosition(), j = 0; i != scroll->getPosition() + m_visibleItems; ++i, ++j){
			if(i >= m_items.size()){
				break;
			}
			
			if(isPointInRectangle(x, y, m_position.x + 1, m_position.y + 1 + j * 12, m_width - SCROLL_ICON_DIMENSION, 12)){
				return select(i);
			}
		}
	}
	
	return false;
}

bool ListBox::onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input){
	if(event == EVENT_KEY_DOWN){
		switch(state){
			case KEYBOARD_STATE_UP: {
				return select(std::max(0, m_selectedItem - 1));
			}
			
			case KEYBOARD_STATE_DOWN: {
				return select(std::min((int)m_items.size() - 1, m_selectedItem + 1));
			}
			
			case KEYBOARD_STATE_ASCII: {
				for(int i = 0; i < m_items.size(); i++){
					std::string* it = &m_items[i];
					
					if(strncasecmp(it->c_str(), (char*)&input, 1) == 0){
						return select(i);
					}
				}
				
				return true;
			}
			
			default: break;
		}
	}
	
	// Dispatch the event among children
	return GUIParent::onKeyboardEvent(event, state, input);
}

int ListBox::selection(){
	return m_selectedItem;
}

std::string ListBox::selectionstr(){
	if(m_selectedItem < m_items.size()){
		return m_items[m_selectedItem];
	}
	
	return std::string();
}

bool ListBox::select(int selection){
	m_selectedItem = selection;
	
	if(m_selectedItem < getControl<Scroll>(GUIParent::CONTROL_1_LOCATION)->getPosition()){
		getControl<Scroll>(GUIParent::CONTROL_1_LOCATION)->setPosition(m_selectedItem);
	}

	if(m_selectedItem + 1 > getControl<Scroll>(GUIParent::CONTROL_1_LOCATION)->getPosition() + m_visibleItems){
		getControl<Scroll>(GUIParent::CONTROL_1_LOCATION)->setPosition(m_selectedItem - m_visibleItems + 1);
	}
	
	/* Notify listeners */
	GUIElement::notify(GUIElement::OnChange);
	
	/* Update ListBox */
	update();
  
  /* Everything went ok */
  return true;
}

void ListBox::removeItems(){
	m_items.clear();
}

void ListBox::removeItem(int position){
	if(m_selectedItem == position){
		m_selectedItem = std::max(0, m_selectedItem - 1);
	}
	
	if(position < m_items.size()){
		m_items.erase(m_items.begin() + position);
	}
	
	/* Update ListBox */
	update();
}

void ListBox::removeItem(std::string item){
	std::vector<std::string>::iterator it = std::find(m_items.begin(), m_items.end(), item);
	if(it != m_items.end()){
		m_items.erase(it);
		
		/* Update ListBox */
		update();
	}
}

void ListBox::addItem(std::string item){
	m_items.push_back(item);
	
	/* Update ListBox */
	update();
}

void ListBox::insertItem(int position, std::string item){
	m_items.insert(m_items.begin() + position, item);
}

void ListBox::update(){
	int hiddenItems = std::max(0, (int)m_items.size() - m_visibleItems);
	
	Scroll* scroll = getControl<Scroll>(GUIParent::CONTROL_1_LOCATION);
	scroll->setMax(hiddenItems);
	
	if(hiddenItems){
		scroll->setSize(std::max(0.2, (double)m_visibleItems / (double)(hiddenItems + m_visibleItems)));
	} else {
		scroll->setSize(1.0);
	}
}

void ListBox::draw(int surface){
	/* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
	
	Painter::drawRectangle(surface, absolute->x, absolute->y, m_width, 12 * m_visibleItems, 64, 64, 64);
	
	Scroll* scroll = getControl<Scroll>(GUIParent::CONTROL_1_LOCATION);
	for(int i = scroll->getPosition(), j = 0; i != scroll->getPosition() + m_visibleItems; ++i, ++j){
		if(i >= m_items.size()){
			break;
		}
		
		if(i == m_selectedItem){
			Painter::drawRectangle(surface, absolute->x + 1, absolute->y + 1 + j * 12, m_width - 14, 12, 112, 112, 112);
			Painter::drawText(surface, absolute->x + 2, absolute->y + 2 + j * 12, FONT_BIG, 254, 254, 254, (char*)m_items[i].c_str(), 0);
		} else {		
			Painter::drawText(surface, absolute->x + 2, absolute->y + 2 + j * 12, FONT_BIG, 175, 175, 175, (char*)m_items[i].c_str(), 0);
		}
	}
	
	Painter::drawBorders(surface, absolute->x - 1, absolute->y - 1, m_width + 2, m_visibleItems * 12 + 1);
}