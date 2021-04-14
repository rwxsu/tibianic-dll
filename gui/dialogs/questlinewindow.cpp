#include "questlinewindow.h"
#include "main.h"

#define QUEST_BOX_W 280
#define QUEST_BOX_H 10

#define QUEST_LIST_BOX_DISPLACEMENT_X 11
#define QUEST_LIST_BOX_DISPLACEMENT_Y 45

#define QUEST_MEMO_BOX_DISPLACEMENT_X 11
#define QUEST_MEMO_BOX_DISPLACEMENT_Y 170

#define QUEST_LABEL_EX_DISPLACEMENT_X 12
#define QUEST_LABEL_EX_DISPLACEMENT_Y 28

GUIQuestLine::GUIQuestLine() : GUIWindow("Quest Information", 0, 0, 300, 328){
	GUIWindow::pushButton("Close", &GUI::pushDialog, g_dll.m_questsDialog, getButtonNextX(), getButtonNextY());
	
	GUIParent::addListBox(CONTROL_1_LOCATION, QUEST_LIST_BOX_DISPLACEMENT_X, QUEST_LIST_BOX_DISPLACEMENT_Y, QUEST_BOX_W, QUEST_BOX_H);
	GUIParent::addMemoBox(CONTROL_2_LOCATION, QUEST_MEMO_BOX_DISPLACEMENT_X, QUEST_MEMO_BOX_DISPLACEMENT_Y, QUEST_BOX_W, QUEST_BOX_H);
	GUIParent::addLabel(CONTROL_3_LOCATION, QUEST_LABEL_EX_DISPLACEMENT_X, QUEST_LABEL_EX_DISPLACEMENT_Y);
	
	GUIParent::getControl<MemoBox>(CONTROL_2_LOCATION)->setText("There is no mission selected.");
	
	/* Add listeners */
	GUIParent::getControl<ListBox>(CONTROL_1_LOCATION)->addListener(GUIElement::OnChange, boost::bind(&GUIQuestLine::selectMission, this, _1));
}

void GUIQuestLine::setQuest(std::string quest){
	GUIParent::getControl<Label>(CONTROL_3_LOCATION)->setText(Label_t(FONT_BIG, 191, 191, 191, 0), quest);
	
}

void GUIQuestLine::setMissions(std::vector<std::string> missions, std::vector<std::string> descriptions){
	m_descriptions = descriptions;
	m_missions = missions;
	
	// Remove previous items
	GUIParent::getControl<ListBox>(CONTROL_1_LOCATION)->select(0);
	GUIParent::getControl<ListBox>(CONTROL_1_LOCATION)->removeItems();
	
	for(std::vector<std::string>::iterator it = m_missions.begin(); it != m_missions.end(); ++it){
		GUIParent::getControl<ListBox>(CONTROL_1_LOCATION)->addItem(*it);
	}
	
	if(m_missions.size() && m_descriptions.size()){
		selectMission(this);
	}
}

void GUIQuestLine::selectMission(GUIElement* caller){
	int selection = GUIParent::getControl<ListBox>(CONTROL_1_LOCATION)->selection();
	
	if(m_descriptions.size() > selection){
		GUIParent::getControl<MemoBox>(CONTROL_2_LOCATION)->setText(m_descriptions[selection]);
	} else {
		GUIParent::getControl<MemoBox>(CONTROL_2_LOCATION)->setText("There is no mission selected.");
	}
}
