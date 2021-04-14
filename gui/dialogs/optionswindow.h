#ifndef __OPTIONSWINDOWH__
#define __OPTIONSWINDOWH__

#include "guiwindow.h"
#include "lock.h"
#include "config.h"

#include <map>

#define CHECKBOX_WIDTH 239
#define CHECKBOX_HEIGHT 22

class GUIOptionsWindow : public GUIWindow {
	friend class LIB;
	
	public:
		static void cancelOptions(Button* button, void* lpParam);
		static void acceptOptions(Button* button, void* lpParam);	
	public:
		GUIOptionsWindow();
		~GUIOptionsWindow();
		
		bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);
		
		void loadConfig();
		void saveConfig();
		
		void setPing(bool value);
		void setWsad(bool value);
		void setExph(bool value);
		void setNfps(bool value);
		void setTogl(bool value);
		void setMana(bool value);
		void setInfo(bool value);
		
		bool getPing();
		bool getWsad();
		bool getExph();
		bool getNfps();
		bool getTogl();
		bool getMana();
		bool getInfo();
	protected:
		//void DrawRectangle(int nSurface, int X, int Y, int W, int H);
		void DrawCheckBox(int nSurface, int X, int Y, char* lpText, bool check);
		void drawSelf(int nSurface);
	private:
		bool m_ping;
		bool m_wsad;
		bool m_exph;
		bool m_nfps;
		bool m_togl;
		bool m_mana;
		bool m_info;
		
		std::string m_pingDescription;
		std::string m_wsadDescription;
		std::string m_exphDescription;
		std::string m_nfpsDescription;
		std::string m_toglDescription;
		std::string m_manaDescription;
		std::string m_infoDescription;
		
		Config config;
};

#endif
