#include "guiparent.h"
#include "scroll.h"
#include "memobox.h"
#include "listbox.h"
#include "buttons.h"
#include "scroll.h"

GUIParent::GUIParent(int x, int y, int width, int height) : GUIElement(x, y, width, height) {
	m_focusChild = NULL;
}

bool GUIParent::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	for(std::vector<GUIElement*>::iterator it = m_children.begin(); it != m_children.end(); ++it){
    GUIElement* element = *it;
    
		if(isButtonClickEvent(event, leftButtonDown)){
			if(isPointInRectangle(x - m_position.x, y - m_position.y, element->m_position.x, element->m_position.y, element->m_width, element->m_height)){
        if(element->focusable()){
          GUIParent::focus(element);
        }
			}
		}
		
    if(element->active()){
      if(element->onMouseEvent(event, x - m_position.x, y - m_position.y, leftButtonDown, rightButtonDown)){
        return true;
      }
    }
	}
	
	return false;
}

bool GUIParent::onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input){
  if(state == KEYBOARD_STATE_TAB){
    if(m_focusChild){
      std::vector<GUIElement*>::iterator it = m_children.begin();
      while(it != m_children.end()){
        GUIElement* child = *it;
        
        if(m_focusChild == NULL){
          if(child->focusable()){
            GUIParent::focus(child);
            break;
          }
          
          if(it + 1 == m_children.end()){
            it = m_children.begin();
            continue;
          }
        }
        
        if(m_focusChild == child){
          GUIParent::focus(NULL);
        }
        
        it++;
      }
    }
    
    /* This kind of action should not be required in elements themselves */
    return true;
  }
  
	if(m_focusChild){
		return m_focusChild->onKeyboardEvent(event, state, input);
	}
	
	for(std::vector<GUIElement*>::iterator it = m_children.begin(); it != m_children.end(); ++it){
    if((*it)->active()){
      if((*it)->onKeyboardEvent(event, state, input)){
        return true;
      }
    }
	}
	
	return false;
}

void GUIParent::focus(GUIElement* child){
	if(m_focusChild){
		m_focusChild->focus(false);
	}
	
	m_focusChild = child;
  
  if(m_focusChild){
    m_focusChild->focus(true);
  }
}

void GUIParent::draw(int surface){
	for(std::vector<GUIElement*>::iterator it = m_children.begin(); it != m_children.end(); ++it){
		if(!(*it)->hidden()){
			(*it)->draw(surface);
		
			if(GUIParent* parent = (*it)->getGUIParent()){
				GUIParent::drawAll(parent, surface);
			}
		}
	}
}

void GUIParent::drawAll(GUIParent* parent, int surface){
	for(std::vector<GUIElement*>::iterator it = parent->m_children.begin(); it != parent->m_children.end(); ++it){
		(*it)->draw(surface);
		
		if(GUIParent* parent = (*it)->getGUIParent()){
			GUIParent::drawAll(parent, surface);
		}
	}
}

void GUIParent::tick(uint32_t ticks){
	for(std::vector<GUIElement*>::iterator it = m_children.begin(); it != m_children.end(); ++it){
		(*it)->tick(ticks);
		
		if(GUIParent* parent = (*it)->getGUIParent()){
			GUIParent::tickAll(parent, ticks);
		}
	}
}

void GUIParent::tickAll(GUIParent* parent, uint32_t ticks){
	for(std::vector<GUIElement*>::iterator it = parent->m_children.begin(); it != parent->m_children.end(); ++it){
		(*it)->tick(ticks);
		
		if(GUIParent* parent = (*it)->getGUIParent()){
			GUIParent::tickAll(parent, ticks);
		}
	}
}

void GUIParent::addScroll(location_t location, int x, int y, Scroll_t type, int min, int max, int length, double size, int step){
	m_locations[location] = addChild(new Scroll(x, y, type, min, max, length, size, step));
}

void GUIParent::addMemoBox(location_t location, int x, int y, int width, int visibleLines){
	m_locations[location] = addChild(new MemoBox(x, y, width, visibleLines));
}

void GUIParent::addListBox(location_t location, int x, int y, int width, int visibleItems){
	m_locations[location] = addChild(new ListBox(x, y, width, visibleItems));
}

void GUIParent::addCheckBox(location_t location, int x, int y, int width, bool border /*= true*/){
	m_locations[location] = addChild(new CheckBox(x, y, width, border));
}

void GUIParent::addButtons(location_t location, int x, int y){
	m_locations[location] = addChild(new Buttons(x, y));
}

void GUIParent::addLabel(location_t location, int x, int y){
	m_locations[location] = addChild(new Label(x, y));
}

void GUIParent::addLabelEx(location_t location, int x, int y, int width){
	m_locations[location] = addChild(new LabelEx(x, y, width));
}

void GUIParent::addProgressBar(location_t location, int x, int y, int width, int height, int min, int max, int red, int green, int blue){
	m_locations[location] = addChild(new ProgressBar(x, y, width, height, min, max, red, green, blue));
}

void GUIParent::addInputBox(location_t location, int x, int y, int width){
	m_locations[location] = addChild(new InputBox(x, y, width));
}

void GUIParent::addSkin(location_t location, int x, int y, int width, int height, int skinId){
	m_locations[location] = addChild(new Skin(x, y, width, height, skinId));
}

int GUIParent::addChild(GUIElement* child){
	m_children.push_back(child);
	child->setParent(this);
  
  if(m_focusChild == NULL){
    if(child->focusable()){
      GUIParent::focus(child);
    }
  }
	
	return m_children.size() - 1;
}

void GUIParent::removeChild(GUIElement* child){
	for(std::vector<GUIElement*>::iterator it = m_children.begin(); it != m_children.end(); ++it){
		if((*it) == child){
			(*it)->setParent(NULL);
			
			m_children.erase(it);
			break;
		}
	}
  
  if(m_focusChild == child){
    GUIParent::focus(NULL);
  }
}
