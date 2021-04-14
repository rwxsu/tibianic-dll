#ifndef __DLLH__
#define __DLLH___

#include "definitions.h"

#include <windows.h>
#include <windowsx.h>
#include <sddl.h>
#include <iostream>
#include <assert.h>

#include "const.h"
#include "pinger.h"
#include "cam.h"
#include "buttons.h"
#include "uitibia.h"
#include "guiwindow.h"
#include "gui.h"
#include "updatewindow.h"
#include "newswindow.h"
#include "optionswindow.h"
#include "questlogwindow.h"
#include "questlinewindow.h"
#include "tradewindow.h"
#include "outfitdialog.h"
#include "testdialog.h"
#include "tools.h"
#include "functions.h"
#include "networkmessage.h"
#include "xtea.h"
#include "rsa.h"
#include "crc.h"

class LIB {
	public:
		LIB();
		~LIB();
		
		// Initialization of the library
		void Initialize();
		void Deinitialize();
		
		// The first tick function which will be called right after first tick from the Tibia timer
		void FirstTick();
		
		// Input events (keyboard, mouse)
		bool onAsciiCharacter(WORD character);
    
    // Cursor validation
    void checkCursorPos();
		
		// Events
		void onConnectEvent(const struct sockaddr* address);
		void onLogoutEvent();
		
		// Memory re-allocation
		void InitializeMemory();
		
		// Login data related functions
		void InitHostLogin(std::string host, uint16_t port);
		void InitLocalHostLogin();
		void InitGlobalHostLogin();
		
		// Network related functions
		void RequestSharedExperience();
		void RequestOutfitDialog();
		void RequestQuestLine(uint16_t questId);
		
		void SendXTEANetworkMessage(NetworkMessage* message);
		void SendRSANetworkMessage(NetworkMessage* message);
		
		// Client visible manipulations
		void SetExperienceText(bool Display, uint32_t Experience = 0);
		
		// Main objects
		void setPinger(Pinger* p){ pinger = p; }
		Pinger* getPinger(){ return pinger; }
		Buttons* getButtons(){ return buttons; }
		CAM* getCAM(){ return cam; }
		
		// Asm related functions
		DWORD HookAsmCall(DWORD dwAddress, DWORD dwFunction, DWORD NOPs = 0);
		void AsmNop(DWORD dwAddress, DWORD dwNops);
		void AsmDword(DWORD dwAddress, DWORD dwFunction);
		
		// Public variables
		GUINewsWindow* m_newsDialog;
		GUIUpdateWindow* m_updateDialog;
		GUIOptionsWindow* m_optionsDialog;
		GUIQuestsLog* m_questsDialog;
		GUIQuestLine* m_questDialog;
		GUITradeWindow* m_tradeDialog;
		GUIOutfitDialog* m_outfitDialog;
		
		NetworkMessage* m_networkMessage;
		
		bool m_gameMenu;
		bool m_gameFps;
		bool m_firstFrame;
		bool m_firstExperience;
		bool m_wsad;
		bool m_escToggle;	
		bool m_manaBar;
		bool m_online;
		bool m_inventory;
		bool m_sharedExperience;
		
		bool m_validHost;
		bool m_validBuffer;
		bool m_validCursor;
		
		Pinger* pinger;
		CAM* cam;
		
		Buttons* buttons;
		Buttons* m_inventoryButtons;
		
		Button* m_gameRecordButton;
		Button* m_gameQuestsButton;
		
		Button* m_inventoryOptsButton;
		Button* m_inventoryNewsButton;
		Button* m_inventoryManaButton;
		Button* m_inventoryPingButton;
		Button* m_inventoryWsadButton;
		
		ArrowSlot* m_arrowSlot;
		
		bool m_displayExperience;
		uint64_t m_beginExperience;
		uint64_t m_beginExpTime;
		
		std::string m_error;
		
		std::string m_pingHost;
		std::string m_moduleName;
		
		uint32_t m_accountNumber;
		std::string m_accountPassword;
		
		int32_t m_screenHeight;
		int32_t m_screenWidth;
		
		HWND m_hWnd;
		WNDPROC m_wndProc;
		SOCKET m_socket;
		char* m_buffer;
		char* m_chunk;
		
		WSADATA wsaData;
	protected:
		void InitializeHook();
		void DeinitializeHook();
		bool InitSecurityAttr();
		bool CreateDACL(SECURITY_ATTRIBUTES* pSA);
	private:
		HANDLE m_currentProcess;
		DWORD m_currentProcessId;
};

#endif
