#ifndef __INTPUTBOXH__
#define __INTPUTBOXH__

#include "guielement.h"
#include "const.h"
#include "tools.h"

#include <iostream>
#include <sstream>

class InputBox : public GUIElement {
	public:
		enum action_t {
			Erase_Left,
			Erase_Right,
			Move_Left,
			Move_Right,
			Insert_Char,
			Paste_Text,
			Copy_Text,
			Select_Text,
			Cut_Text
		};
		
	public:
		InputBox(int x, int y, int width);
		
		/* Override mouse and keyboard events */
		bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);
		bool onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input);
		
		/* Basic functions */
		std::string getText();
		
		/* Override draw */
		void draw(int surface);
	protected:
		/* At which letter are we pointing at? */
		int32_t letter(int32_t sx);
		
		/* Create cache */
		void cache();
		
		/* Create cached text */
		void cache_forwards(bool recreate = false);
		
		/* Parse action */
		bool action(action_t action, int param = 0);
	private:	
		std::string m_text; // whole text
		std::string m_cacheText; // visible text
		
		int32_t m_cacheLocation; // Position of the cached text in m_text
		int32_t m_cacheCursor; // Cursor in cached text
		int32_t m_cacheLength; // Length of the cached text
		int32_t m_cacheSelection; // How many characters are selected relatively to m_cacheCursor (-4 or 4 - 4 to the left or to the right)
		int32_t m_cacheClick; // Last click position
		
		bool m_textSelected; // Whether the text was selected
		
		// Helper variables for cursor actions
		bool m_increaseCursor;
		bool m_decreaseCursor;
};

#endif
