#ifndef __LABELH__
#define __LABELH__

#include "guielement.h"
#include "functions.h"
#include "tools.h"

#include <iostream>
#include <vector>
#include <algorithm>

class Label : public GUIElement {
	public:
		Label(int x, int y);
		
		/* Set new text */
		void setText(Label_t label, std::string text);
		void setText(std::string text);
		
		/* Virtual function */
		void draw(int surface);
	private:
		std::string m_text;
		Label_t m_label;
};

class LabelEx : public GUIElement {
	public:
		LabelEx(int x, int y, int width);
		
		/* Set new text */
		void setText(Label_t label, std::string text);
		void setText(std::string text);
		
		/* Virtual function */
		void draw(int surface);
	private:
		std::vector<std::string> m_lines;
		Label_t m_label;
};

#endif
