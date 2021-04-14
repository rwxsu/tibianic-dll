#include "questlogwindow.h"
#include "main.h"

#define QUESTS_LIST_BOX_DISPLACEMENT_X 11
#define QUESTS_LIST_BOX_DISPLACEMENT_Y 25

void GUIQuestsLog::showQuest(Button* button, void* param){
	GUIQuestsLog* quests = reinterpret_cast<GUIQuestsLog*>(param);
	ListBox* listbox = quests->getControl<ListBox>(GUIParent::CONTROL_1_LOCATION);
	
	g_dll.m_questDialog->setQuest(listbox->selectionstr());
	g_dll.RequestQuestLine(quests->m_questsIds[listbox->selection()]);
}

GUIQuestsLog::GUIQuestsLog() : GUIWindow("Quests Log", 0, 0, 300, 308){
	GUIWindow::pushButton("Close", &GUI::nullDialog, this, getButtonNextX(), getButtonNextY());
	GUIWindow::pushButton("Show Quest", &GUIQuestsLog::showQuest, this, getButtonNextX(), getButtonNextY());
	
	GUIParent::addListBox(GUIParent::CONTROL_1_LOCATION, QUESTS_LIST_BOX_DISPLACEMENT_X, QUESTS_LIST_BOX_DISPLACEMENT_Y, 280, 20);
}

void GUIQuestsLog::setQuests(std::vector<std::string> questsNames, std::vector<int> questsIds){
	m_questsIds = questsIds;
	
	GUIParent::getControl<ListBox>(GUIParent::CONTROL_1_LOCATION)->removeItems();
	GUIParent::getControl<ListBox>(GUIParent::CONTROL_1_LOCATION)->select(0);
	
	for(std::vector<std::string>::iterator it = questsNames.begin(); it != questsNames.end(); ++it){
		GUIParent::getControl<ListBox>(GUIParent::CONTROL_1_LOCATION)->addItem(*it);
	}
	
	if(!m_questsIds.empty()){
		GUIParent::getControl<Buttons>(GUIParent::BUTTONS_1_LOCATION)->getButton(2)->enable();
	} else {	
		GUIParent::getControl<Buttons>(GUIParent::BUTTONS_1_LOCATION)->getButton(2)->disable();
	}
}
