#include "optionswindow.h"
#include "gui.h"
#include "buttons.h"
#include "networkmessage.h"
#include "tools.h"
#include "crc.h"
#include "httpget.h"
#include "thread.h"
#include "main.h"

#include <windows.h>
#include <winsock.h>
#include <windowsx.h>
#include <stdio.h>
#include <sstream>

bool gamePing(bool active){
	Pinger* pinger = g_dll.getPinger();
	if(pinger == NULL){
		pinger = new Pinger(g_dll.m_pingHost);
		g_dll.setPinger(pinger);
	}
	
	if(!pinger->available()){
    //std::string error = "Ping in game is not available on this computer! Error:\r\n" + pinger->getLastError();  
    //Tibia::GUIDialog* dialog = Tibia::CreateTextDialog("Information", error.c_str());
		//Tibia::SetCurrentDialog(dialog);
		return false;
	}

	if(active){ 
		pinger->start();
	} else {
		pinger->stop();
	}
	
	return true;
}

void GUIOptionsWindow::cancelOptions(Button* button, void* lpParam){
	GUIOptionsWindow* options = reinterpret_cast<GUIOptionsWindow*>(lpParam);
	
	options->m_wsad = g_dll.m_wsad;
	options->m_togl = g_dll.m_escToggle;
	
	g_gui->setDialog(NULL);
}

void GUIOptionsWindow::acceptOptions(Button* button, void* lpParam){
	GUIOptionsWindow* options = reinterpret_cast<GUIOptionsWindow*>(lpParam);
	
	g_dll.m_wsad = options->m_wsad;
	g_dll.m_escToggle = options->m_togl;
	
	g_gui->setDialog(NULL);
}

GUIOptionsWindow::GUIOptionsWindow() : GUIWindow("Advanced Tibia options", 0, 0, 260, 304){
	pushButton("Cancel", &GUIOptionsWindow::cancelOptions, this, getButtonNextX(), getButtonNextY());
	pushButton("Ok", &GUIOptionsWindow::acceptOptions, this, getButtonNextX(), getButtonNextY());
	
	m_pingDescription = std::string("Display ping in game");
	m_wsadDescription = std::string("Switch walking keys to W/A/S/D");
	m_exphDescription = std::string("Display exp/h on title bar");
	m_nfpsDescription = std::string("Display frames per second");
	m_toglDescription = std::string("Toggle W/A/S/D by ESC key");
	m_manaDescription = std::string("Display mana bar above character");
	m_infoDescription = std::string("Display CAM menu");
}

GUIOptionsWindow::~GUIOptionsWindow(){
	saveConfig();
}

void GUIOptionsWindow::loadConfig(){
	config.load(std::string(GAME_DIR) + std::string(MAIN_ADVANCED_CONFIG));
	
	setPing(config.getBoolean("PingInGame"));
	setWsad(config.getBoolean("WASDWalking"));
	setExph(config.getBoolean("ExpCounter"));
	setNfps(config.getBoolean("FpsInGame"));
	setTogl(config.getBoolean("ESCToggle"));
	setMana(config.getBoolean("ManaBar"));
	setInfo(config.getBoolean("MenuCAM"));
	
	config.free();
}

void GUIOptionsWindow::saveConfig(){
	config.setBoolean("PingInGame", m_ping);
	config.setBoolean("WASDWalking", m_wsad);
	config.setBoolean("ExpCounter", m_exph);
	config.setBoolean("FpsInGame", m_nfps);
	config.setBoolean("ESCToggle", m_togl);
	config.setBoolean("ManaBar", m_mana);
	config.setBoolean("MenuCAM", m_info);
	
	config.save();
}

void GUIOptionsWindow::setPing(bool value){
	m_ping = value;
	
	if(!gamePing(m_ping)){
		if(m_ping){
			m_ping = false;
		}
	}
	
	g_dll.m_inventoryPingButton->setActive(m_ping);
}

void GUIOptionsWindow::setWsad(bool value){
	m_wsad = value;
	g_dll.m_wsad = m_wsad;
	
	g_dll.m_inventoryWsadButton->setActive(m_wsad);
}

void GUIOptionsWindow::setExph(bool value){
	m_exph = value;
	
	if(m_exph){
		g_dll.m_beginExperience = *(uint32_t*)PLAYER_EXP_ADDRESS;
		g_dll.m_beginExpTime = GetTickCount();
		g_dll.SetExperienceText(m_exph, *(uint32_t*)PLAYER_EXP_ADDRESS);
	} else {
		g_dll.SetExperienceText(false);
	}
	
	g_dll.m_displayExperience = m_exph;
}

void GUIOptionsWindow::setNfps(bool value){
	m_nfps = value;
	g_dll.m_gameFps = m_nfps;
}

void GUIOptionsWindow::setTogl(bool value){
	m_togl = value;
	g_dll.m_escToggle = m_togl;
}

void GUIOptionsWindow::setMana(bool value){
	m_mana = value;
	g_dll.m_manaBar = m_mana;
	
	g_dll.m_inventoryManaButton->setActive(m_mana);
}

void GUIOptionsWindow::setInfo(bool value){
	m_info = value;
	g_dll.m_gameMenu = m_info;
	
	if(m_info){
		g_dll.buttons->enable();
	} else {
		g_dll.buttons->disable();
	}
}

bool GUIOptionsWindow::getPing(){
	return m_ping;
}

bool GUIOptionsWindow::getWsad(){
	return m_wsad;
}

bool GUIOptionsWindow::getExph(){
	return m_exph;
}

bool GUIOptionsWindow::getNfps(){
	return m_nfps;
}

bool GUIOptionsWindow::getTogl(){
	return m_togl;
}

bool GUIOptionsWindow::getMana(){
	return m_mana;
}

bool GUIOptionsWindow::getInfo(){
	return m_info;
}

bool GUIOptionsWindow::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	if(GUIWindow::onMouseEvent(event, x, y, leftButtonDown)){
		return true;
	}
	
	if(isButtonClickEvent(event, leftButtonDown)){
		if(isPointInRectangle(x, y, m_position.x + 13, m_position.y + 25, CHECKBOX_WIDTH, CHECKBOX_HEIGHT)){
			setPing(!m_ping);
			return true;
		}
		
		if(isPointInRectangle(x, y, m_position.x + 13, m_position.y + 25 + 36*1, CHECKBOX_WIDTH, CHECKBOX_HEIGHT)){
			setWsad(!m_wsad);
			return true;
		}
		
		if(isPointInRectangle(x, y, m_position.x + 13, m_position.y + 25 + 36*2, CHECKBOX_WIDTH, CHECKBOX_HEIGHT)){
			m_togl = !m_togl;
			return true;
		}
		
		if(isPointInRectangle(x, y, m_position.x + 13, m_position.y + 25 + 36*3, CHECKBOX_WIDTH, CHECKBOX_HEIGHT)){
			setExph(!m_exph);
			return true;
		}
		
		if(isPointInRectangle(x, y, m_position.x + 13, m_position.y + 25 + 36*4, CHECKBOX_WIDTH, CHECKBOX_HEIGHT)){
			setNfps(!m_nfps);
			return true;
		}
		
		if(isPointInRectangle(x, y, m_position.x + 13, m_position.y + 25 + 36*5, CHECKBOX_WIDTH, CHECKBOX_HEIGHT)){
			setMana(!m_mana);
			return true;
		}
		
		if(isPointInRectangle(x, y, m_position.x + 13, m_position.y + 25 + 36*6, CHECKBOX_WIDTH, CHECKBOX_HEIGHT)){
			setInfo(!m_info);
			return true;
		}
	}
	
	return false;
}

void GUIOptionsWindow::DrawCheckBox(int nSurface, int X, int Y, char* lpText, bool check){
	Painter::drawBorders(nSurface, m_position.x + X, m_position.y + Y, CHECKBOX_WIDTH, CHECKBOX_HEIGHT);
	Painter::drawSkin(nSurface, m_position.x + X + 6, m_position.y + Y + 5, 12, 12, check ? 150 : 151, 0, 0);
	Painter::drawText(nSurface, m_position.x + X + 22, m_position.y + Y + 7, FONT_BIG, 175, 175, 175, lpText, 0);
}

void GUIOptionsWindow::drawSelf(int nSurface){
	DrawCheckBox(nSurface, 13, 25, (char*)m_pingDescription.c_str(), m_ping);	
	DrawCheckBox(nSurface, 13, 25 + 36*1, (char*)m_wsadDescription.c_str(), m_wsad);
	DrawCheckBox(nSurface, 13, 25 + 36*2, (char*)m_toglDescription.c_str(), m_togl);
	DrawCheckBox(nSurface, 13, 25 + 36*3, (char*)m_exphDescription.c_str(), m_exph);
	DrawCheckBox(nSurface, 13, 25 + 36*4, (char*)m_nfpsDescription.c_str(), m_nfps);
	DrawCheckBox(nSurface, 13, 25 + 36*5, (char*)m_manaDescription.c_str(), m_mana);
	DrawCheckBox(nSurface, 13, 25 + 36*6, (char*)m_infoDescription.c_str(), m_info);
}
