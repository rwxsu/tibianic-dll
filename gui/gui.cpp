#include "gui.h"
#include "main.h"
#include "tools.h"

#include <sstream>

GUI::GUI(){
	m_drag = NULL;
	m_dialog = NULL;
}

void GUI::draw(uint32_t surface){
	for(std::vector<GUIWindow*>::iterator it = m_windows.begin(); it != m_windows.end(); ++it){
		(*it)->draw(surface);
	}
	
	if(m_dialog){
		m_dialog->draw(surface);
	}
}

void GUI::tick(uint32_t ticks){
	/* Tick current dialog */
	if(m_dialog){
		m_dialog->tick(ticks);
	}
	
	/* Tick floating windows */
	for(std::vector<GUIWindow*>::iterator it = m_windows.begin(); it != m_windows.end(); ++it){
		(*it)->tick(ticks);
	}
	
	/* Tick standalone elements */
	for(std::vector<GUIElement*>::iterator it = m_elements.begin(); it != m_elements.end(); ++it){
		(*it)->tick(ticks);
	}
}

bool GUI::hasDialog(){
	return m_dialog != NULL;
}

void GUI::setDialog(GUIWindow* dialog){
	m_dialog = dialog;
	
	if(m_dialog){
		m_dialog->onResolution();
	}
	
	/* Set Tibia dialog */
	if(dialog != NULL){
		Tibia::SetCurrentDialog(NULL);
	}
}

GUIWindow* GUI::getDialog(){
	return m_dialog;
}

void GUI::nullDialog(Button* button, void* caller){
	g_gui->setDialog(NULL);
}

void GUI::pushDialog(Button* button, void* dialog){
	g_gui->setDialog((GUIWindow*)dialog);
}

void GUI::pushElement(GUIElement* element){
	m_elements.push_back(element);
}

void GUI::releaseElement(GUIElement* element){
	for(std::vector<GUIElement*>::iterator it = m_elements.begin(); it != m_elements.end(); ++it){
		if((*it) == element){
			m_elements.erase(it);
			break;
		}
	}
}

void GUI::releaseElements(){
	for(std::vector<GUIElement*>::iterator it = m_elements.begin(); it != m_elements.end(); ++it){
		delete (*it);
	}
	
	m_elements.clear();
}

void GUI::pushWindow(GUIWindow* window){
	m_windows.push_back(window);
}

void GUI::releaseWindow(GUIWindow* window){
	for(std::vector<GUIWindow*>::iterator it = m_windows.begin(); it != m_windows.end(); ++it){
		if((*it) == window){
			m_windows.erase(it);
			break;
		}
	}
}

void GUI::releaseWindows(){
	for(std::vector<GUIWindow*>::iterator it = m_windows.begin(); it != m_windows.end(); ++it){
		delete *it;
	}
	
	m_windows.clear();
}

void GUI::onResolution(){
	for(std::vector<GUIWindow*>::iterator it = m_windows.begin(); it != m_windows.end(); ++it){
		(*it)->onResolution();
	}
	
	if(m_dialog){
		m_dialog->onResolution();
	}
}

bool GUI::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	if(m_drag){
		switch(event){
			case EVENT_MOVEMENT: {
				m_drag->m_position.x = std::max(0, std::min(g_dll.m_screenWidth - m_drag->m_width, x - m_click.x));
				m_drag->m_position.y = std::max(0, std::min(g_dll.m_screenHeight - m_drag->m_height, y - m_click.y));		
				return true;
			}
			
			default: break;
		}
		
		m_drag = NULL;
	}
	
	/* Set current position if it's button click event */
	if(isButtonClickEvent(event, leftButtonDown)){
		m_click = Position(x, y, 0);
	}
	
	/* Is there a Tibia dialog? */
	//if(Tibia::HasDialog()){
	//	return false;
	//}
	
	/* Check events for dialog */
	if(m_dialog){
		if(m_dialog->onMouseEvent(event, x, y, leftButtonDown, rightButtonDown)){
			return true;
		}
			
		if(isButtonClickEvent(event, leftButtonDown)){
			if(isPointInRectangle(x, y, m_dialog->m_position.x, m_dialog->m_position.y, m_dialog->m_width, 18)){
				m_drag = m_dialog;
				m_click.x = x - m_drag->m_position.x;
				m_click.y = y - m_drag->m_position.y;
				return true;
			}
		}
		
		/* In any way the dialog will block everything */
		return true;
	}
	
	/* Check events for standalone elements */
	for(std::vector<GUIElement*>::iterator it = m_elements.begin(); it != m_elements.end(); ++it){
		if(!(*it)->active()){
			continue;
		}
		
		if((*it)->onMouseEvent(event, x, y, leftButtonDown)){
			return true;
		}
	}
	
	/* Check events for available windows */
	for(std::vector<GUIWindow*>::reverse_iterator it = m_windows.rbegin(); it != m_windows.rend(); ++it){
		if((*it)->onMouseEvent(event, x, y, leftButtonDown)){
			return true;
		}
		
		if(isButtonClickEvent(event, leftButtonDown)){
			if(isPointInRectangle(x, y, (*it)->m_position.x, (*it)->m_position.y, (*it)->m_width, 18)){
				m_drag = *it;
				m_click.x = x - m_drag->m_position.x;
				m_click.y = y - m_drag->m_position.y;
				return true;
			}
		}
		
		break;
	}
	
	return false;
}

bool GUI::onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input){
	if(m_dialog){
		return m_dialog->onKeyboardEvent(event, state, input);
	}
	
	/* Is there a Tibia dialog? */
	//if(Tibia::HasDialog()){
	//	return false;
	//}
	
	// Check events for standalone elements
	for(std::vector<GUIElement*>::iterator it = m_elements.begin(); it != m_elements.end(); ++it){
		if(!(*it)->active()){
			continue;
		}
		
		if((*it)->onKeyboardEvent(event, state, input)){
			return true;
		}
	}
	
	for(std::vector<GUIWindow*>::reverse_iterator it = m_windows.rbegin(); it != m_windows.rend(); ++it){
		if((*it)->onKeyboardEvent(event, state, input)){
			return true;
		}
		
		break;
	}
	
	return false;
}
