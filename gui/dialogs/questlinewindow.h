#ifndef __QUESTLINEH__
#define __QUESTLINEH__

#include <vector>
#include <map>

#include "const.h"
#include "guiwindow.h"
#include "listbox.h"
#include "memobox.h"

class GUIQuestLine : public GUIWindow {
	public:
		GUIQuestLine();
		
		/* Static functions */
		static void mission(ListBox* listbox);
		
		/* Quest related functions */
		void setQuest(std::string quest);
		void setMissions(std::vector<std::string> missions, std::vector<std::string> descriptions);
	protected:
		void selectMission(GUIElement* caller);
	private:
		std::vector<std::string> m_missions;
		std::vector<std::string> m_descriptions;
};

#endif
