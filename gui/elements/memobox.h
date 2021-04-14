#ifndef __MEMOBOXH__
#define __MEMOBOXH__

#include "guiparent.h"
#include "scroll.h"
#include "networkmessage.h"
#include "functions.h"

#include <vector>
#include <algorithm>

class MemoBox : public GUIParent {
	public:
		MemoBox(int x, int y, int width, int visibleLines);
		
		/* Override virtual functions */
		bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);	
		bool onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input);
		
		/* MemoBox functions */
		void setText(std::string text);		
		void draw(int nSurface);
	private:
		int m_visibleLines;
		std::vector<std::string> m_lines;
};

#endif
