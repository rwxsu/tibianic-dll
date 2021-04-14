#ifndef __QUESTSLOGWINDOWH__
#define __QUESTSLOGWINDOWH__

#include <vector>
#include <map>

#include "const.h"
#include "guiwindow.h"
#include "listbox.h"

class GUIQuestsLog : public GUIWindow {
	public:
		GUIQuestsLog();
		
		/* Custom methods */
		void setQuests(std::vector<std::string> questsNames, std::vector<int> questsIds);
		
		/* Static methods */
		static void showQuest(Button* button, void* param);
	private:
		std::vector<int> m_questsIds;
};

#endif
