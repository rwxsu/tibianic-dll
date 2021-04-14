#include "progressbar.h"

ProgressBar::ProgressBar(int x, int y, int width, int height, int min, int max, int red, int green, int blue) : GUIElement(x, y, width, height) {
	m_color = Color_t(red, green, blue);
	
	// Minimum and maximum values of the progress bar
	m_min = min;
	m_max = max;
	
	// Current position
	m_pos = 0;
}

void ProgressBar::step(){
	m_pos = std::max(m_min, std::min(m_pos + 1, m_max));
}

void ProgressBar::back(){
	m_pos = std::max(m_min, std::min(m_pos - 1, m_max));
}

void ProgressBar::step(int steps){
	m_pos = std::max(m_min, std::min(m_pos + steps, m_max));
}

void ProgressBar::back(int steps){
	m_pos = std::max(m_min, std::min(m_pos - steps, m_max));
}

void ProgressBar::pos(int position){
	m_pos = std::max(m_min, std::min(position, m_max));
}

void ProgressBar::draw(int surface){
	/* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
	
	Painter::drawRectangle(surface, absolute->x, absolute->y, m_width, m_height, 0, 0, 0);
	
	int advance = m_pos * (m_width - 2) / (m_max - m_min);
	if(advance){
		Painter::drawRectangle(surface, absolute->x + 1, absolute->y + 1, advance, m_height - 2, m_color.red, m_color.green, m_color.blue);
	}
}
