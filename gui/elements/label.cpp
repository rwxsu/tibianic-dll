#include "label.h"

/* Normal label without cutting the string into lines */

Label::Label(int x, int y) : GUIElement(x, y, 0, 0) {

}

void Label::setText(Label_t label, std::string text){
	m_label = label;
	m_text = text;
	
	/* Set new dimensions of the element */
	GUIElement::setWidth(Tibia::TextMetric(m_label.font, (char*)text.c_str(), text.length()));
	GUIElement::setHeight(14);
}

void Label::setText(std::string text){
	m_text = text;
	
	/* Set new dimensions of the element */
	GUIElement::setWidth(Tibia::TextMetric(m_label.font, (char*)text.c_str(), text.length()));
	GUIElement::setHeight(14);
}

void Label::draw(int surface){
	/* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
	
	Painter::drawText(surface, absolute->x, absolute->y, m_label.font, m_label.red, m_label.green, m_label.blue, (char*)m_text.c_str(), m_label.align);
}

/* Extended label with possibility to cut into lines */

LabelEx::LabelEx(int x, int y, int width) : GUIElement(x, y, width, 0) {

}

void LabelEx::setText(Label_t label, std::string text){
	m_label = label;
	m_lines = wrapTextLines(m_label.font, text, m_width);
	
	/* Set new dimensions of the element */
	GUIElement::setWidth(Tibia::TextMetric(m_label.font, (char*)text.c_str(), text.length()));
	GUIElement::setHeight(m_lines.size() * 14);
}

void LabelEx::setText(std::string text){
	m_lines = wrapTextLines(m_label.font, text, m_width);
	
	/* Set new dimensions of the element */
	GUIElement::setWidth(Tibia::TextMetric(m_label.font, (char*)text.c_str(), text.length()));
	GUIElement::setHeight(m_lines.size() * 14);
}

void LabelEx::draw(int surface){
	/* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
	
	for(int j = 0; j < m_lines.size(); j++){
		Painter::drawText(surface, absolute->x, absolute->y + j * 14, m_label.font, m_label.red, m_label.green, m_label.blue, (char*)m_lines[j].c_str(), m_label.align);
	}
}
