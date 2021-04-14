#ifndef __LISTBOXH__
#define __LISTBOXH__

#include "scroll.h"
#include "networkmessage.h"
#include "functions.h"
#include "guiparent.h"

#include <vector>
#include <algorithm>

class ListBox : public GUIParent {
	public:
		ListBox(int x, int y, int width, int visibleItems);
		~ListBox();
		
		/* Virtual functions replaced with our own */
		bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);
		bool onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input);

		/* ListBox selected item related functions */
		int selection();
		std::string selectionstr();
		bool select(int selection);	
		
		/* ListBox items related functions */
		void removeItems();
		void removeItem(int position);
		void removeItem(std::string item);
		
		void addItem(std::string item);
		void insertItem(int position, std::string item);
		
		/* Virtual draw function */
		void draw(int surface);
	protected:
		void update();
	private:
		int m_visibleItems;
		int m_selectedItem;
		
		std::vector<std::string> m_items;
};

#endif
