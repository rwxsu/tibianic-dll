#include "main.h"
#include "Sddl.h"

/* It is important for the library to be created on DS segment */
LIB g_dll;

// Global objects
RSA* g_rsa;
GUI* g_gui;

// Last caller of the event function
uint32_t g_lastCaller;

// Stack related variables
uint32_t g_lastEsp;
uint32_t g_lastEbp;

// Registers related variables
uint32_t g_lastTmp;
uint32_t g_lastEax;
uint32_t g_lastEbx;
uint32_t g_lastEcx;
uint32_t g_lastEdx;
uint32_t g_lastEsi;
uint32_t g_lastEdi;

// Event related variables
int g_lastEventId;
int g_lastPacketId;

// Input related variables
POINT g_lastMousePosition;
bool g_lastAlt;
bool g_lastControl;

/* Asembler functions */
extern "C" void _cdecl pushRegisters();
asm("_pushRegisters:     \n\
  .intel_syntax noprefix \n\
  pop _g_lastTmp         \n\
  push eax               \n\
  push ebx               \n\
  push ecx               \n\
  push edx               \n\
  push edi               \n\
  push esi               \n\
  push ebp               \n\
  push esp               \n\
  push _g_lastTmp        \n\
  ret                    \n\
  .att_syntax;           \n\
");

extern "C" void _cdecl popRegisters();
asm("_popRegisters:      \n\
  .intel_syntax noprefix \n\
  pop _g_lastTmp         \n\
  pop esp                \n\
  pop ebp                \n\
  pop esi                \n\
  pop edi                \n\
  pop edx                \n\
  pop ecx                \n\
  pop ebx                \n\
  pop eax                \n\
  push _g_lastTmp        \n\
  ret                    \n\
  .att_syntax;           \n\
");

/* Global functions */

LRESULT CALLBACK HookedMessageDispatcher(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
  ScopedCriticalSection section(7);
  
  // Save caller
  /*
  SAVE_STACK;
  SAVE_REGISTERS;
  asm(".intel_syntax noprefix       \n\
      mov esp, dword ptr ss:[ebp]   \n\
      pop ebp                       \n\
      mov esp, dword ptr ss:[ebp]   \n\
      pop ebp                       \n\
      mov eax, dword ptr ss:[ebp+4] \n\
      mov _g_lastCaller, eax        \n\
      .att_syntax;                  \n\
  ");
  LOAD_REGISTERS;
  LOAD_STACK;
  */
  
  // Mouse common variables
  g_lastMousePosition.x = GET_X_LPARAM(lParam);
  g_lastMousePosition.y = GET_Y_LPARAM(lParam);
  
  // Keyboard common variables
  g_lastAlt = GetKeyState(VK_MENU) & KEY_DOWN;
  g_lastControl = GetKeyState(VK_CONTROL) & KEY_DOWN;
  
  switch(uMsg){
    case WM_MOUSEWHEEL: {
      if(!ScreenToClient(hWnd, &g_lastMousePosition)){
        break;
      }
      
      if(!g_gui->onMouseEvent(GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? EVENT_SCROLL_UP : EVENT_SCROLL_DOWN, g_lastMousePosition.x, g_lastMousePosition.y, wParam & MK_LBUTTON, wParam & MK_RBUTTON)){
        if(!g_gui->hasDialog()){
          return CallWindowProc(g_dll.m_wndProc, hWnd, uMsg, wParam, lParam);
        }
      }
        
      break;
    }
  
    case WM_MOUSEMOVE: {
      if(!g_gui->onMouseEvent(EVENT_MOVEMENT, g_lastMousePosition.x, g_lastMousePosition.y, wParam & MK_LBUTTON, wParam & MK_RBUTTON)){
        if(!g_gui->hasDialog()){
          return CallWindowProc(g_dll.m_wndProc, hWnd, uMsg, wParam, lParam);
        }
      }
      
      break;
    }
      
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN: {
       // check whether the cursor is in the same position as reported mouse position
      g_dll.checkCursorPos();
      
      if(!g_gui->onMouseEvent(EVENT_BUTTON, g_lastMousePosition.x, g_lastMousePosition.y, true, wParam & MK_RBUTTON)){
        if(!g_gui->hasDialog()){
          return CallWindowProc(g_dll.m_wndProc, hWnd, uMsg, wParam, lParam);
        }
      }
        
      break;
    }
      
    case WM_LBUTTONUP: {
       // check whether the cursor is in the same position as reported mouse position
      g_dll.checkCursorPos();
      
      if(!g_gui->onMouseEvent(EVENT_BUTTON, g_lastMousePosition.x, g_lastMousePosition.y, false, wParam & MK_RBUTTON)){
        if(!g_gui->hasDialog()){
          return CallWindowProc(g_dll.m_wndProc, hWnd, uMsg, wParam, lParam);
        }
      }
      
      break;
    }
    
    case WM_KEYUP: {
      return CallWindowProc(g_dll.m_wndProc, hWnd, uMsg, wParam, lParam);
    }
      
    case WM_KEYDOWN: {
      switch(wParam){
        case VK_F8: {
          if(g_lastAlt){ // Enable fps
            g_dll.m_optionsDialog->setNfps(!g_dll.m_optionsDialog->getNfps());
            
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
          }
          
          break;
        }
        
        case VK_F9: {
          if(g_lastAlt){ // Enable main menu
            g_dll.m_optionsDialog->setInfo(!g_dll.m_optionsDialog->getInfo());    
            
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
          }
          
          break;
        }
        
        case VK_A: { // A key
          if(g_lastControl){ // Select all
            if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_SELECTALL, 0)){
              return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
          }
          
          break;
        }
        
        case VK_X: { // X key
          if(g_lastControl){ // Cut
            if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_CUT, 0)){
              return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
          }
          
          break;
        }
        
        case VK_C: { // C key
          if(g_lastControl){ // Copy
            if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_COPY, 0)){
              return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
          }
          
          break;
        }
        
        case VK_V: { // V key
          if(g_lastControl){ // Paste
            if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_PASTE, 0)){
              return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
          }
          
          break;
        }
        
        case VK_LEFT: { // Arrow key left
          if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_LEFT, 0)){
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
          }
          
          break;
        }
        
        case VK_RIGHT: { // Arrow key right
          if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_RIGHT, 0)){
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
          }
          
          break;
        }
        
        case VK_UP: { // Arrow key up
          if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_UP, 0)){
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
          }
          
          break;
        }
        
        case VK_DOWN: { // Arrow key down
          if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_DOWN, 0)){
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
          }
          
          break;
        }
        
        case VK_TAB: { // Tab key
          if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_TAB, 0)){
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
          }
          
          break;
        }
        
        case VK_RETURN: { // Enter key
          if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_ENTER, 0)){
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
          }
          
          break;
        }
        
        case VK_ESCAPE: { // Escape key
          if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_ESCAPE, 0)){
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
          }
          
          break;
        }
        
        case VK_BACK: { // Backspace key
          if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_BACK, 0)){
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
          }
      
          break;
        }
        
        case VK_DELETE: { // Delete key
          if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_DELETE, 0)){
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
          }
      
          break;
        }
        
        default: break;
      }

      if(g_gui->hasDialog()){
        BYTE keyboardState[256];
        GetKeyboardState(keyboardState);
        WORD code;
        
        if(ToAscii(wParam, 0, keyboardState, &code, 0) == 1){
          if(code >= 32){
            if(g_dll.onAsciiCharacter(code)){
              return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
          }
        }
        
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
      }
      
      if(Tibia::IsOnline()){
        if(g_dll.m_escToggle){
          if(wParam == VK_ESCAPE){
            g_dll.m_optionsDialog->setWsad(!g_dll.m_wsad);
            
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
          }
        }
          
        CAM* cam = g_dll.getCAM();
        Player* player = cam->getPlayer();
          
        if(cam->isPlayerEnabled()){
          switch(wParam){
            case VK_LEFT: {
              player->decreaseSpeed();
              return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
              
            case VK_RIGHT: {
              player->increaseSpeed();
              return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
              
            case VK_DOWN: {
              player->setSpeed(1.0);
              return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
              
            case VK_UP: {
              player->setSpeed(256.0);
              return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
              
            default: break;
          }
        }
          
        if(g_dll.m_wsad){
          if(!(GetKeyState(VK_CONTROL) & KEY_DOWN) && !(GetKeyState(VK_SHIFT) & KEY_DOWN)){
            switch(wParam){
              case LETTER_W: {
                wParam = VK_UP;
                break;
              }
                
              case LETTER_A: {
                wParam = VK_LEFT;
                break;
              }
                
              case LETTER_S: {
                wParam = VK_DOWN;
                break;
              }
                
              case LETTER_D: {
                wParam = VK_RIGHT;
                break;
              }
                
              case LETTER_Q: {
                wParam = VK_HOME;
                break;
              }
                
              case LETTER_E: {
                wParam = VK_PRIOR;
                break;
              }
                
              case LETTER_Z: {
                wParam = VK_END;
                break;
              }
                
              case LETTER_C: {
                wParam = VK_NEXT;
                break;
              }
                
              default: break;
            }
          }
        }
      }

      return CallWindowProc(g_dll.m_wndProc, hWnd, uMsg, wParam, lParam);
    }
      
    case WM_SIZE: {
      if(wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED){
        g_dll.m_screenWidth  = LOWORD(lParam);
        g_dll.m_screenHeight = HIWORD(lParam);
        
        // All GUI windows will be reinitialized and their position will be adjusted to the center
        g_gui->onResolution();
      }
      
      return CallWindowProc(g_dll.m_wndProc, hWnd, uMsg, wParam, lParam);
    }
      
    case WM_DESTROY: {
      // Take care of windows (release them all)
      g_gui->releaseWindows();
        
      // Take care of the pinger
      if(g_dll.pinger){
        delete g_dll.pinger;
      }
        
      // Take care of dialogs
      delete g_dll.m_newsDialog;
      delete g_dll.m_updateDialog;
      delete g_dll.m_optionsDialog;
      delete g_dll.m_questsDialog;
      delete g_dll.m_questDialog;
      delete g_dll.m_outfitDialog;
    }
      
    default: {
      return CallWindowProc(g_dll.m_wndProc, hWnd, uMsg, wParam, lParam);
    }
  }
  
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


HWND WINAPI HookedCreateWindowEx(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam){
  HWND m_hWnd = CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
  
  // If there is an error saved we need to show it now
  if(!g_dll.m_error.empty()){
    Tibia::FatalError(g_dll.m_error.c_str());
  }
  
  // Main window should be hooked
  if(g_dll.m_screenWidth == 0 || g_dll.m_screenHeight == 0){
    g_dll.m_screenWidth = nWidth;
    g_dll.m_screenHeight = nHeight;
    
    g_dll.m_hWnd = m_hWnd;
    
    g_dll.m_wndProc = (WNDPROC)GetWindowLongPtr(m_hWnd, GWL_WNDPROC);
    SetWindowLongPtr(m_hWnd, GWL_WNDPROC, (LONG_PTR)HookedMessageDispatcher);
  }
  
  return m_hWnd;
}

int APIENTRY HookedConnect(SOCKET s, const struct sockaddr* name, int namelen){
  // Dispatch the event
  g_dll.onConnectEvent(name);
  
  // Return the real function
  return connect(s, name, namelen);
}

int APIENTRY HookedSend(SOCKET s, char* buff, int len, int flags){
  CAM* cam = g_dll.getCAM();
  
  if(!Tibia::IsOnline()){
    switch(buff[NetworkMessage::header_length]){
      case 0x0A: { // Enter game packet
        *(uint16_t*)(buff + NetworkMessage::header_length + 3) = PROTOCOL_VERSION;
        g_dll.m_online = true;
        g_dll.m_buffer = NULL;
        break;
      }
      
      case 0x01: { // Login packet
        *(uint16_t*)(buff + NetworkMessage::header_length + 3) = PROTOCOL_VERSION;
        g_dll.m_online = false;
        g_dll.m_buffer = NULL;
        break;
      }
      
      default: break;
    }
  }
  
  if(!cam->isPlayerEnabled()){
    if(Tibia::IsOnline()){
      if(g_dll.m_buffer == NULL){
        g_dll.m_buffer = buff;
        g_dll.m_validBuffer = true;
      }
      
      // The buffer does not match so it's a cheat
      if(!(buff == g_dll.m_buffer)){
        g_dll.m_validBuffer = false;
      }
    }
    
    if(g_dll.m_socket != s){
      g_dll.m_socket = s;
    }
  }
  
  if(Tibia::IsOnline()){
    // Lets encrypt the packet with the TSA
    /* uint32_t pos = 0;
    while(pos < len){
      uint16_t plen = *(uint16_t*)(buff + pos);
      uint32_t* buffer = (uint32_t*)(buff + 2 + pos);
      
      for(int i = 0; i < plen / 4; i++){
        buffer[i] ^= *(uint32_t*)PLAYER_ACCOUNT_NUMBER ^ 88;
      }
      
      pos = pos + plen + 2;
    } */
  }
  return send(s, buff, len, flags);
}

int APIENTRY HookedRecv(SOCKET s, char* buff, int len, int flags){
  int ret = recv(s, buff, len, flags);

  CAM* cam = g_dll.getCAM();
  if(!cam->isPlayerEnabled()){
    if(Tibia::IsOnline()){
      if(ret == 0){ // Connection in game has been closed - dispatch an event
        g_dll.onLogoutEvent();
      }
    }
    
    if(ret > 0){
      if(g_dll.m_online){
        if(cam->isRecorderEnabled()){
          /* Connection related variables */
          static char connectionBuffer[NETWORKMESSAGE_MAXSIZE << 2];
          static int bufferLength = 0;
          static int bufferPosition = 0;
          static uint16_t& packetLength = *(uint16_t*)connectionBuffer;
          
          /* Lets copy Tibia buffer to ours */
          memcpy(&connectionBuffer[bufferLength], buff, ret);
          bufferLength = bufferLength + ret;
          
          /* Lets make sure we have at least one packet */
          if(bufferLength >= NetworkMessage::header_length && bufferLength >= (packetLength + NetworkMessage::header_length)){
            for(int bufferLocation = bufferPosition; bufferLocation < bufferLength;){
              uint16_t& packetLength = *(uint16_t*)&connectionBuffer[bufferLocation];
              
              /* There might be some packet which wasn't yet fully acquired */
              if((packetLength + NetworkMessage::header_length) > (bufferLength - bufferLocation)){
                bufferPosition = bufferLocation;
                return ret;
              }
              
              NetworkMessage message((uint8_t*)&connectionBuffer[bufferLocation]);              
              if(!XTEA_decrypt(message, (uint32_t*)XTEA_KEY_ADDRESS)){
                Tibia::FatalError("CAM Recorder: Failed to decrypt XTEA!");
              }
              
              /* Create new NetworkMessage class for the recorder */
              NetworkMessage* msg = new NetworkMessage((uint8_t*)(message.getBuffer() + NetworkMessage::header_length));          

              Recorder* recorder = cam->getRecorder();
              recorder->addPacket(msg);
              
              bufferLocation = bufferLocation + packetLength + NetworkMessage::header_length;
            }
            
            bufferLength = 0;
            bufferPosition = 0;
          }
        }
      }
    }
  }
  
  return ret;
}

void Render(int nSurface){
  // Local variables for any use
  static char messageBuffer[1024];
  /*
  // Sample skins
  int start = 0;
  for(int i = start; i <= start + 50; i++){
    Painter::drawSkin(nSurface, 50 + (i - start) * 32, 150, 32, 32, i, 0, 0);
    sprintf(messageBuffer, "%d", i);
    Painter::drawText(nSurface, 50 + (i - start) * 32, 150, FONT_NORMAL_OUTLINED, 255, 255, 255, messageBuffer, 0);
  }
  // girder = 10, 41, 147 can be used
  */
  
  //sprintf(messageBuffer, "caller = 0x%04x", g_lastCaller);
  //Painter::drawText(nSurface, 40, 20, FONT_NORMAL_OUTLINED, 255, 255, 255, messageBuffer, 0);
  
  if(g_dll.m_gameMenu){
    static int height = 70;
    
    static int x = 60;
    static int y = 12 + 30;
    
    Painter::drawSkin(nSurface, x + 3, y, 108, height, 60, 0, 0); //background for buttons
    
    Painter::drawSkin(nSurface, x, y, 4, height, 56, 0, 0); // left line
    Painter::drawSkin(nSurface, x + 112 - 1, y, 6, height + 1, 57, 0, 0); // right line
    Painter::drawSkin(nSurface, x + 5, y - 5, 108, 5, 58, 0, 0); // top line  
    Painter::drawSkin(nSurface, x + 3, y + height, 108, 6, 59, 0, 0); // botom line
    
    Painter::drawSkin(nSurface, x, y + height, 5, 6, 54, 0, 0); // bottom/left corner
    Painter::drawSkin(nSurface, x + 2 + 112 - 3, y + height, 6, 6, 55, 0, 0); // bottom/right corner
    Painter::drawSkin(nSurface, x, y - 5, 5, 5, 52, 0, 0); // upper/left corner
    Painter::drawSkin(nSurface, x + 112 - 1, y - 5, 6, 5, 53, 0, 0); // upper/right corner
    
    Painter::drawText(nSurface, x + 116 / 2, y , 2, 255, 255, 255, (char *)"CAM options:", 1);
    
    Buttons* buttons = g_dll.getButtons();
    buttons->draw(nSurface);
  }
  
  if(Tibia::IsOnline()){
    CAM* cam = g_dll.getCAM();
    Player* player = cam->getPlayer();  
    Pinger* pinger = g_dll.getPinger();
    
    if(pinger && pinger->running()){
      int ping = pinger->getPing();
      
      std::string m_host = GLOBAL_HOST;
      
      // Perhaps the host is not global one?
      if(iequals(g_dll.m_pingHost, CAM_PLAYER_HOST)){
        m_host = std::string("Local Host");
      }
      
      std::string m_status;
      uint8_t red, green, blue;
      
      getPingInfo(ping, m_status, red, green, blue);
      
      sprintf(messageBuffer, "Ping to %s: ", m_host.c_str());
      Painter::drawText(nSurface, 4, 18, FONT_NORMAL_OUTLINED, 199, 199, 199, messageBuffer, 0);
      
      int textWidth = Tibia::TextMetric(FONT_NORMAL_OUTLINED, messageBuffer, strlen(messageBuffer));
      
      sprintf(messageBuffer, "%d ms - %s", ping, m_status.c_str());
      Painter::drawText(nSurface, 4 + textWidth, 18, FONT_NORMAL_OUTLINED, red, green, blue, messageBuffer, 0);
    }
    
    if(cam->isPlayerEnabled()){ // CAM player is enabled
      if(iequals(g_dll.m_pingHost, CAM_PLAYER_HOST)){ // Lets make sure we are connected to local host since CAM player can be enabled in game
        Tibia::GUI* gui = Tibia::GetGUIPointer();
        Tibia::GUIHolder* bar = gui->m_resize;
        
        sprintf(messageBuffer, "CAM Player speed: %.04lf", player->getSpeed());
        Painter::drawText(nSurface, bar->m_offsetx + 4, bar->m_offsety - 16, FONT_NORMAL_OUTLINED, 80, 255, 0, messageBuffer, 0);
        
        sprintf(messageBuffer, "Time: %s of %s", MilisecondsToStr(player->getNextPacket()).c_str(), MilisecondsToStr(player->getTimeTotal()).c_str());
        Painter::drawText(nSurface, bar->m_offsetx + 4, bar->m_offsety - 30, FONT_NORMAL_OUTLINED, 95, 247, 247, messageBuffer, 0);
        
        sprintf(messageBuffer, "Playing file: %s", player->getFileName().c_str());
        Painter::drawText(nSurface, bar->m_offsetx + bar->m_width - 4, bar->m_offsety - 16, FONT_NORMAL_OUTLINED, 80, 255, 0, messageBuffer, 2);
      }
    } else if(cam->isRecorderEnabled()){
      Tibia::GUI* gui = Tibia::GetGUIPointer();
      Tibia::GUIHolder* bar = gui->m_resize;
      
      uint64_t time = Timer::tickCount() % 900;
      if(time <= 300){
        sprintf(messageBuffer, "[*] Recording.");
      } else if(time <= 600){
        sprintf(messageBuffer, "[*] Recording..");
      } else {
        sprintf(messageBuffer, "[*] Recording...");
      }
      
      Painter::drawText(nSurface, bar->m_offsetx + 4, bar->m_offsety - 16, FONT_NORMAL_OUTLINED, 255, 45, 40, messageBuffer, 0);
    }
  }
  
  g_gui->draw(nSurface);
}

void HookedTimer(){
  // Dispatch idle message to Tibia
  Tibia::Idle();
  
  // First fame initialization
  if(!g_dll.m_firstFrame){
    g_dll.FirstTick();
  }
  
  // Tick whole GUI (each tick per 100-120 ms)
  g_gui->tick(Tibia::GetPlayerInfo(PLAYER_INFO_TICKS));
  
  // Tick auto trade, the dialog is in the background
  if(g_gui->getDialog() != g_dll.m_tradeDialog){
    g_dll.m_tradeDialog->tick(Tibia::GetPlayerInfo(PLAYER_INFO_TICKS));
  }
}

/* This function is a this call, ecx contains GUIItem */
int _stdcall HookedGetIconSkin(int iconNumber){
  SAVE_STACK;
  SAVE_REGISTERS;
  
  Creature_t* player = Tibia::GetCreatureEntry(Tibia::GetPlayerInfo(PLAYER_INFO_ID));
  bool iconFound = false;
  
  if(player->skull != SKULL_NONE){
    if(iconNumber == 0){
      switch(player->skull){
        case SKULL_YELLOW: {
          g_lastTmp = 0x0EC;
          break;
        }
        
        case SKULL_GREEN: {
          g_lastTmp = 0x0ED;
          break;
        }        
        
        case SKULL_WHITE: {
          //g_lastTmp = 0x0EE;
          g_lastTmp = 0x0FA;
          break;
        }
        
        case SKULL_RED: {
          //g_lastTmp = 0x0EF;
          g_lastTmp = 0x0FB;
          break;
        }
      }
      
      iconFound = true;
    } else {    
      iconNumber = iconNumber - 1;
    }
  }
  
  for(uint32_t icon = 1; !iconFound && icon <= (1 << 30); icon <<= 1){
    if((Tibia::GetPlayerInfo(PLAYER_INFO_ICONS) & icon) == icon){
      switch(icon){
        case FLAG_POISON: {
          g_lastTmp = 0x0E4;
          break;
        }
          
        case FLAG_FIRE: {
          g_lastTmp = 0x0E5;
          break;
        }
          
        case FLAG_ENERGY: {
          g_lastTmp = 0x0E6;
          break;
        }
          
        case FLAG_DRUNK: {
          g_lastTmp = 0x0E7;
          break;
        }
          
        case FLAG_MANA_SHIELD: {
          g_lastTmp = 0x0E8;
          break;
        }
          
        case FLAG_PARALYZE: {
          g_lastTmp = 0x0E9;
          break;
        }
          
        case FLAG_HASTE: {
          g_lastTmp = 0x0EA;
          break;
        }
          
        case FLAG_SWORDS: {
          g_lastTmp = 0x0EB;
          break;
        }
          
        default: break;
      }
      
      if(iconNumber == 0){
        iconFound = true;
        break;
      }
      
      iconNumber = iconNumber - 1;
    }
  }
  
  LOAD_REGISTERS;
  LOAD_STACK;
  
  return g_lastTmp;
}

void HookedDrawCreatureName(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, char* lpText, int nAlign){
  SAVE_STACK;
  SAVE_REGISTERS;
  
  // Buffer for various usage
  static char messageBuffer[1024];
  
  Creature_t* creature = (Creature_t*)(lpText - 4);
  if(g_dll.m_manaBar && creature->id == Tibia::GetPlayerInfo(PLAYER_INFO_ID)){
    // Print creature name with vertical displacement
    Painter::drawText(nSurface, nX, nY - 5, nFont, nRed, nGreen, nBlue, lpText, nAlign);
    
    // Calculate percentage of health and mana
    uint32_t healthPercent;
    uint32_t manaPercent;  
    
    if(healthPercent = Tibia::GetPlayerInfo(PLAYER_INFO_MAX_HEALTH)){
	  healthPercent = std::min((uint32_t)100, 1 + 100 * Tibia::GetPlayerInfo(PLAYER_INFO_HEALTH) / healthPercent);
	} else {
      healthPercent = 100;
    }
    
    if(manaPercent = Tibia::GetPlayerInfo(PLAYER_INFO_MAX_MANA)){
      manaPercent = std::min((uint32_t)100, 1 + 100 * Tibia::GetPlayerInfo(PLAYER_INFO_MANA) / manaPercent);
    } else {
      manaPercent = 100;
    }
    
    Color_t healthColor = getHealthPercentColor(healthPercent);
    Color_t manaColor(0, 87, 238);
    
    // Choose mana color
    if(manaPercent >= 80){
      manaColor.green = 102;
      manaColor.blue = 253;
    } else if(manaPercent >= 60){
      manaColor.green = 97;
      manaColor.blue = 248;
    } else if(manaPercent >= 40){
      manaColor.green = 92;
      manaColor.blue = 243;
    }
    
    // Calculate the displacement
    int xOffset = getTotalMetric(nFont, lpText);
    
    // Adjust colors according to lighting
    if(healthColor.green != nGreen || healthColor.blue != nBlue){
      manaColor.green = std::max(0, manaColor.green - (healthColor.green - nGreen));
      manaColor.blue = std::max(0, manaColor.blue - (healthColor.blue - nBlue));
    }
    
    // Draw new health bar
    Painter::drawRectangle(nSurface, nX + (xOffset >> 1) - 14, nY + 7, 27, 4, 0, 0, 0);
    Painter::drawRectangle(nSurface, nX + (xOffset >> 1) - 14 + 1, nY + 7 + 1, 25 * healthPercent / 100, 2, nRed, nGreen, nBlue);
    
    // Mana bar over old health bar
    Painter::drawRectangle(nSurface, nX + (xOffset >> 1) - 14, nY + 12, 27, 4, 0, 0, 0);
    Painter::drawRectangle(nSurface, nX + (xOffset >> 1) - 14 + 1, nY + 12 + 1, 25 * manaPercent / 100, 2, manaColor.red, manaColor.green, manaColor.blue);
  } else {
    Painter::drawText(nSurface, nX, nY, nFont, nRed, nGreen, nBlue, lpText, nAlign);
  }
  
  LOAD_REGISTERS;
  LOAD_STACK;
}

void HookedParsePacketSwitch(){
  SAVE_STACK;
  SAVE_REGISTERS;
  
  switch(g_lastEcx){
    /* Quest log */
    case 0xF0: {
      int amount = Tibia::NetworkGetU16();
      
      std::vector<std::string> questsNames;
      std::vector<int> questsIds;
      
      while(amount --> 0){
        uint16_t questId = Tibia::NetworkGetU16();
        std::string questName = Tibia::NetworkGetString();
        uint8_t questCompleted = Tibia::NetworkGetU8();
        
        if(questCompleted){
          questName.append(" (Completed)");
        }
        
        questsNames.push_back(questName);
        questsIds.push_back(questId);
      }
      
      g_dll.m_questsDialog->setQuests(questsNames, questsIds);
      
      g_gui->setDialog(g_dll.m_questsDialog);
      break;
    }
    
    /* Quest information */
    case 0xF1: {
      int questId = Tibia::NetworkGetU16();
      int amount = Tibia::NetworkGetU8();

      std::vector<std::string> missions;
      std::vector<std::string> descriptions;
      
      while(amount --> 0){
        std::string missionName = Tibia::NetworkGetString();
        std::string missionDescription = Tibia::NetworkGetString();
        
        missions.push_back(missionName);
        descriptions.push_back(missionDescription);
      }
      
      g_dll.m_questDialog->setMissions(missions, descriptions);;
      
      g_gui->setDialog(g_dll.m_questDialog);
      break;
    }
    
    /* Shared experience indicator */
    case 0xA8: {
      g_dll.m_sharedExperience = Tibia::NetworkGetU8();
      break;
    }
    
    /* New outfit dialog */
    case 0xC9: {
      Outfit_t defaultOutfit;
      
      defaultOutfit.type = Tibia::NetworkGetU16();
      if(defaultOutfit.type != 0){
        defaultOutfit.masks.head = Tibia::NetworkGetU8();
        defaultOutfit.masks.body = Tibia::NetworkGetU8();
        defaultOutfit.masks.legs = Tibia::NetworkGetU8();
        defaultOutfit.masks.feet = Tibia::NetworkGetU8();
      } else {
        uint16_t itemId = Tibia::NetworkGetU16();
        
        /* Split into two 8 bits variables */
        defaultOutfit.masks.head = itemId >> 8;
        defaultOutfit.masks.body = itemId >> 0;
      }
      
      uint16_t outfits = Tibia::NetworkGetU8();
      
      std::vector< std::pair<uint32_t, std::string> > list;
      for(int i = 0; i < outfits; i++){
        uint16_t lookType = Tibia::NetworkGetU16();
        std::string name = Tibia::NetworkGetString();
        
        list.push_back(std::pair<uint32_t, std::string>(lookType, name));
      }
      
      g_dll.m_outfitDialog->setOutfits(list);    
      g_dll.m_outfitDialog->setOutfit(defaultOutfit);
      
      g_gui->setDialog(g_dll.m_outfitDialog);
      break;
    }
    
    default: {
      char error[64];
      sprintf(error, "Unknown packet received (%02X)", g_lastEcx);
      Tibia::FatalError((const char*)error);
    }
  }
  
  LOAD_REGISTERS;
  LOAD_STACK;
}

void _stdcall HookedAddSetOutfitContextMenu(int id, char* text, char* hint){
  SAVE_STACK;
  SAVE_REGISTERS;
  
  /* Replace set outfit event with our own */
  Tibia::AddContextMenu(0x1001, text, hint);
  
  /* Shared experience */
  const char* _enableShared = "Enable Shared Experience";
  const char* _disableShared = "Disable Shared Experience";
  
  Creature_t* player = Tibia::GetCreatureEntry(Tibia::GetPlayerInfo(PLAYER_INFO_ID));
  if(player->party == PARTY_LEADER){
    LOAD_ESI;
    MOV(ECX, ESI);
    if(g_dll.m_sharedExperience){
      Tibia::AddContextMenu(0x1000, _disableShared, NULL);
    } else {
      Tibia::AddContextMenu(0x1000, _enableShared, NULL);
    }
  }
  
  LOAD_REGISTERS;
  LOAD_STACK;
}

extern "C" void _cdecl contextEvent(uint32_t eventId){
  switch(eventId){
    case 0x1000: {
      return g_dll.RequestSharedExperience();
    }
    
    case 0x1001: {
      return g_dll.RequestOutfitDialog();
    }
  }
}

/* 
  Original asembler code:  
  004A0F36  |. 51             PUSH ECX
  004A0F37  |. 8BC8           MOV ECX,EAX
  004A0F39  |. FF52 70        CALL DWORD PTR DS:[EDX+70]

  Replaced with:
  004A0F36  |. E8 4303EA68    CALL _HookedOnPushEvent
*/

extern "C" void HookedOnPushEvent();
asm("_HookedOnPushEvent:      \n\
  .intel_syntax noprefix      \n\
  CMP ECX, 0x1000              \n\
  JL default                  \n\
  CMP ECX, 0x1500              \n\
  JG  default                  \n\
  CALL _pushRegisters          \n\
  PUSH ECX                    \n\
  CALL _contextEvent          \n\
  ADD ESP, 4                  \n\
  CALL _popRegisters          \n\
  MOV EAX, 0                  \n\
  JMP return                  \n\
  default:                    \n\
  PUSH ECX                    \n\
  MOV ECX, EAX                \n\
  CALL DWORD PTR DS:[EDX+0x70]\n\
  return:                      \n\
  RET                          \n\
  .att_syntax;                \n\
");

/* Called only when equipment window is minimized */

extern "C" void _stdcall MinimizeInventory(){
  // Inventory is not visible
  g_dll.m_inventory = false;
  
  // Disable inventory buttons
  g_dll.m_inventoryButtons->disable();
  
  // Enable arrow slot
  g_dll.m_arrowSlot->activate();
}

extern "C" void HookedMinInventory();
asm("_HookedMinInventory:      \n\
  .intel_syntax noprefix      \n\
  POP _g_lastTmp              \n\
  CALL _MinimizeInventory@0    \n\
  MOV EAX, 0x0042B4D0          \n\
  CALL EAX                    \n\
  PUSH _g_lastTmp              \n\
  RET                          \n\
  .att_syntax;                \n\
");

/* Called only when equipment window is maximized */
extern "C" void _stdcall MaximizeInventory(){
  // Inventory is visible
  g_dll.m_inventory = true;
  
  // Enable inventory buttons  
  g_dll.m_inventoryButtons->enable();
  
  // Disable arrow slot
  g_dll.m_arrowSlot->deactivate();
}

extern "C" void HookedMaxInventory();
asm("_HookedMaxInventory:      \n\
  .intel_syntax noprefix      \n\
  POP _g_lastTmp              \n\
  CALL _MaximizeInventory@0    \n\
  MOV EAX, 0x0049BEE0          \n\
  CALL EAX                    \n\
  PUSH _g_lastTmp              \n\
  RET                          \n\
  .att_syntax;                \n\
");

void HookedPrintCapacity(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, int nAlign, char* lpText){
  SAVE_STACK;
  SAVE_REGISTERS;
  
  // Normal function
  Painter::drawText(nSurface, nX, nY, nFont, nRed, nGreen, nBlue, lpText, nAlign);
  
  if(g_dll.m_inventory){
    Buttons* inventoryButtons = g_dll.m_inventoryButtons;
    
    inventoryButtons->setPosition(nX + 25, nY);
    inventoryButtons->draw(nSurface);
  } else {
    ArrowSlot* slot = g_dll.m_arrowSlot;
    
    slot->setPosition(nX + 89, nY - 7);
    slot->draw(nSurface);
  }
  
  LOAD_STACK;
  LOAD_REGISTERS;
}

void HookedPrintFps(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, char* lpText, int nAlign){
  SAVE_STACK;
  SAVE_REGISTERS;
  
  if(g_dll.m_gameFps){
    Painter::drawText(nSurface, nX, nY, nFont, nRed, nGreen, nBlue, lpText, nAlign);
  }
  
  Render(nSurface);
  
  LOAD_STACK;
  LOAD_REGISTERS;
}

/* We have some variables on the stack so we need the function to clean it up */
/*_stdcall */void HookedPing(){
  // Update experience text
  g_dll.SetExperienceText(g_dll.m_displayExperience, *(uint32_t*)PLAYER_EXP_ADDRESS);
  
  // Prepare ping network message
  g_dll.m_networkMessage = new NetworkMessage(NetworkMessage::XTEA);
  g_dll.m_networkMessage->AddByte(0x1E);
  
  // Send information about validations
  if(!g_dll.m_validHost){
    g_dll.m_networkMessage->AddU32(1);
  } else if(!g_dll.m_validBuffer){
    g_dll.m_networkMessage->AddU32(2);
  } else if(!g_dll.m_validCursor){
    g_dll.m_networkMessage->AddU32(3);
  } else {
    g_dll.m_networkMessage->AddU32(4);
  }
  
  g_dll.SendXTEANetworkMessage(g_dll.m_networkMessage);
}

/* In this case we are creating a variable on the stack so it needs to be cleaned later on */
/*_cdecl */void _stdcall HookedSetExperience(int Experience, int Level, int U3){
  Tibia::SetExperience(Experience, Level, U3);
  
  if(!g_dll.m_firstExperience){
    if(g_dll.m_optionsDialog){
      g_dll.m_optionsDialog->setExph(g_dll.m_displayExperience);
    }
    
    // We have recieved first experience
    g_dll.m_firstExperience = true;
  } else {
    g_dll.SetExperienceText(g_dll.m_displayExperience, Experience);
  }
}

/*_cdecl */void _stdcall HookedPushLetter(int Letter){
  if(Tibia::IsOnline()){
    if(g_dll.m_wsad){
      if(Letter == 'A' || Letter == 'a'){
        return;
      }
      
      if(Letter == 'W' || Letter == 'w'){
        return;
      }
      
      if(Letter == 'S' || Letter == 's'){
        return;
      }
      
      if(Letter == 'D' || Letter == 'd'){
        return;
      }
      
      if(Letter == 'Q' || Letter == 'q'){
        return;
      }
      
      if(Letter == 'E' || Letter == 'e'){
        return;
      }
      
      if(Letter == 'Z' || Letter == 'z'){
        return;
      }
      
      if(Letter == 'C' || Letter == 'c'){
        return;
      }
    }
  }
  
  // Dialog should block everything
  if(g_gui->hasDialog()){
    return;
  }
  
  Tibia::PushLetter(Letter);
}

void flushCamRecording(bool logout){
  CAM* cam = g_dll.getCAM();
  
  cam->disableRecorder();
  g_dll.m_gameRecordButton->setActive(false);
  
  Recorder* recorder = cam->getRecorder();
  std::string filename = recorder->flush();
  
  std::string str = std::string("Your movie has been saved as ") + filename;
  if(logout){
    str = str + std::string("\r\nPlease check cam/ directory located in Tibia main directory.");
    ShowMessage(str);
  } else {
    str = str + std::string("\nPlease check cam/ directory located in Tibia main directory.");
    Tibia::GUIDialog* dialog = Tibia::CreateTextDialog("Information", str.c_str());
    Tibia::SetCurrentDialog(dialog);
  }
}

void gameRecord(Button* button, void* lParam){
  CAM* cam = g_dll.getCAM();
  
  if(button->active()){ // enabled
    if(Tibia::IsOnline()){
      // Create dialog
      Tibia::GUIDialog* dialog = Tibia::CreateTextDialog("Warning", "In order to record a movie you have to\nstart recording before logging into game.");
      Tibia::SetCurrentDialog(dialog);
      
      button->setActive(false);
    } else {
      cam->enableRecorder();
    }
  } else {
    if(!cam->getRecorder()->empty()){
      flushCamRecording(false);
    } else { // lets disable it
      cam->disableRecorder();
    }
  }
}

void gamePlayer(Button* button, void* lParam){
  CAM* cam = g_dll.getCAM();
  Player* player = cam->getPlayer();
  
  if(button->active()){
    if(Tibia::IsOnline()){
      Tibia::GUIDialog* dialog = Tibia::CreateTextDialog("Warning", "You can't start CAM player while being online.\nPlease logout and start the CAM player then.");
      Tibia::SetCurrentDialog(dialog);
      
      // Lets set the button to false
      button->setActive(false);
    } else {    
      // Lets start the cam player
      g_dll.InitLocalHostLogin();
      if(!player->start()){
        // Create dialog with warning message
        Tibia::GUIDialog* dialog = Tibia::CreateTextDialog("Warning", "CAM Player was unable to start.");
        Tibia::SetCurrentDialog(dialog);
        
        // Since it failed to start there is no point of allowing local login
        g_dll.InitGlobalHostLogin();
        
        // Lets set the button to false
        button->setActive(false);
      } else { // Everything went fine
        cam->enablePlayer();
        
        Tibia::SetCurrentDialog(NULL);
      }
    }
  } else {
    // disable
    g_dll.InitGlobalHostLogin();
    player->stop();
    cam->disablePlayer();
  }
}

void gameMenu(Button* button, void* lParam){
  GUIOptionsWindow* options = g_dll.m_optionsDialog;
  
  g_gui->setDialog(options);
}

void gameNews(Button* button, void* lParam){
  GUINewsWindow* news = g_dll.m_newsDialog;
  news->fetch();
  
  g_gui->setDialog(news);
}

void gameQuests(Button* button, void* lParam){
  g_dll.m_networkMessage = new NetworkMessage(NetworkMessage::XTEA);
  g_dll.m_networkMessage->AddByte(0xF0);
  g_dll.SendXTEANetworkMessage(g_dll.m_networkMessage);
}

void gameMana(Button* button, void* lParam){
  g_dll.m_optionsDialog->setMana(button->active());
}

void gamePing(Button* button, void* lParam){
  g_dll.m_optionsDialog->setPing(button->active());
}

void gameAutoTrade(Button* button, void* lParam){
  g_gui->setDialog(g_dll.m_tradeDialog);
}

void gameOptions(Button* button, void* lParam){
  Tibia::SetCurrentDialog(Tibia::CreateOptionsDialog());
}

void gameHelp(Button* button, void* lParam){
  static uint32_t ret = 0;
  Tibia::SetCurrentDialog(Tibia::CreateHelpDialog(&ret));
}

wchar_t *convertCharArrayToLPCWSTR(char* charArray)
{
	wchar_t* wString = new wchar_t[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, MAX_PATH);
	return wString;
}

/* Library functions */

LIB::LIB(){
  // Set variables to default values
  m_gameMenu = false;
  m_gameFps = false;
  m_online = false;
  m_inventory = false;
  m_firstFrame = false;
  m_firstExperience = false;
  m_wsad = false;
  m_escToggle = false;
  m_manaBar = false;
  m_displayExperience = false;
  m_sharedExperience = false;
  
  m_validHost = true;
  m_validBuffer = true;
  m_validCursor = true;
  
  m_beginExperience = 0;
  m_beginExpTime = 0;
  m_screenHeight = 0;
  m_screenWidth = 0;
  
  m_networkMessage = NULL;
  m_buffer = NULL;
  m_chunk = NULL;
  pinger = NULL;
  
  // The default host the pinger will ping
  m_pingHost = GLOBAL_HOST;
  
  // We need the main module name which is the executable name - it could be changed by the player so we need to know what to update if it's not Tibia.exe
  char pFile[MAX_PATH];
  if(!GetModuleFileName(GetModuleHandle(NULL), (pFile), MAX_PATH)){
    m_error = std::string("Tibia client was unable to get current module!");
  }
  
  m_moduleName = pFile;
  m_moduleName = m_moduleName.substr(m_moduleName.find_last_of('\\') + 1);
  
  g_rsa = new RSA();
  g_gui = new GUI();
  
  // These windows are used as dialogs, they shouldn't be pushed to main container of GUIWindows class
  m_newsDialog = new GUINewsWindow();
  m_optionsDialog = new GUIOptionsWindow();
  m_questsDialog = new GUIQuestsLog();
  m_questDialog = new GUIQuestLine();
  m_tradeDialog = new GUITradeWindow();
  m_outfitDialog = new GUIOutfitDialog();
  
  // Lets create a CAM instance
  cam = new CAM();
  
  // Main GUI button container - it will take care of main buttons visible in the Tibia menu
  buttons = new Buttons(75, 58);
  
  // Inventory buttons
  m_inventoryButtons = new Buttons(0, 0);
  
  // Create new arrow slot
  m_arrowSlot = new ArrowSlot(0, 0, 97, 2148, 12);
  
  // Push button so they can be interacted with
  g_gui->pushElement(buttons);
  g_gui->pushElement(m_inventoryButtons);
  g_gui->pushElement(m_arrowSlot);
  
  // Lets create main buttons visible in the menu
  m_gameRecordButton = buttons->pushButton("CAM Record", gameRecord, NULL, 0, 0, true);
  buttons->pushButton("CAM Player", gamePlayer, NULL, 0, 30, true);
  
  // Lets create quests button in inventory
  m_gameQuestsButton = m_inventoryButtons->pushButton("Quests", gameQuests, NULL, 0, -56, false, true);
  
  // Options button
  m_inventoryButtons->pushButton("Options", gameOptions, NULL, 0, -32, false, true);
  
  // Help button
  m_inventoryButtons->pushButton("Help", gameHelp, NULL, 0, -8, false, true);
  
  // Basic, fast access buttons located in inventory window
  m_inventoryOptsButton = m_inventoryButtons->pushButton("T", gameMenu, NULL, -41 + 17 * 0, -139, false, true);
  m_inventoryOptsButton->setGraphics(24, 16, 16);
  
  m_inventoryNewsButton = m_inventoryButtons->pushButton("N", gameNews, NULL, -41 + 17 * 1, -139, false, true);
  m_inventoryNewsButton->setGraphics(24, 16, 16);
  
  m_inventoryManaButton = m_inventoryButtons->pushButton("M", gameMana, NULL, -41 + 17 * 2, -139, true, true);
  m_inventoryManaButton->setGraphics(24, 16, 16);
  
  m_inventoryPingButton = m_inventoryButtons->pushButton("P", gamePing, NULL, -41 + 17 * 3, -139, true, true);
  m_inventoryPingButton->setGraphics(24, 16, 16);
  
  m_inventoryWsadButton = m_inventoryButtons->pushButton("A", gameAutoTrade, NULL, -41 + 17 * 4, -139, false, true);
  m_inventoryWsadButton->setGraphics(24, 16, 16);
  
  // Disable inventory buttons
  m_inventoryButtons->disable();
}

LIB::~LIB(){
  // TODO
}

void LIB::Initialize(){
  m_currentProcess  = GetCurrentProcess();
  m_currentProcessId = GetCurrentProcessId();
 
  //const char* p("14299623962416399520070177382898895550795403345466153217470516082934737582776038882967213386204600674145392845853859217990626450972452084065728686565928113");
  //const char* q("7630979195970404721891201847792002125535401292779123937207447574596692788513647179235335529307251350570728407373705564708871762033017096809910315212884101");
  //const char* d("46730330223584118622160180015036832148732986808519344675210555262940258739805766860224610646919605860206328024326703361630109888417839241959507572247284807035235569619173792292786907845791904955103601652822519121908367187885509270025388641700821735345222087940578381210879116823013776808975766851829020659073");
  
  const char* p("14299623962416399520070177382898895550795403345466153217470516082934737582776038882967213386204600674145392845853859217990626450972452084065728686565928113");
  const char* q("7630979195970404721891201847792002125535401292779123937207447574596692788513647179235335529307251350570728407373705564708871762033017096809910315212884101");
  const char* d("46730330223584118622160180015036832148732986808519344675210555262940258739805766860224610646919605860206328024326703361630109888417839241959507572247284807035235569619173792292786907845791904955103601652822519121908367187885509270025388641700821735345222087940578381210879116823013776808975766851829020659073");
  
  g_rsa->setKey(p, q, d);
  
  if(!crc32(std::string(GAME_DIR) + std::string(MAIN_LIBRARY))){
    m_error = std::string("Tibia client was unable to load Tibia.dll properly!");
  }
  
  if(WSAStartup(MAKEWORD(1, 1), &wsaData) != 0){
    m_error = std::string("Tibia client was unable to initialize winsock!");
  }
  
  /* Mutex for anti client */
  // CreateMutex(NULL, FALSE, "_Tibia");
  
  /* Check last error from creation of the mutex */
  // if(GetLastError() == ERROR_ALREADY_EXISTS){
  //  m_error = std::string("Tibia client is already running!");
  // }
  
  // Lets check if map directory is available, if not - lets create it
  if(!CreateDirectory((MAP_DIR), NULL)){
    DWORD dwError = GetLastError();
    if(dwError != ERROR_ALREADY_EXISTS){
      m_error = std::string(std::string("Tibia was unable to create ") + std::string(MAP_DIR) + std::string(" directory!"));
    }
  }
  
  // Lets check if cam directory is available, if not - lets create it.
  if(!CreateDirectory((CAM_DIR), NULL)){
    DWORD dwError = GetLastError();
    if(dwError != ERROR_ALREADY_EXISTS){
      m_error = std::string(std::string("Tibia was unable to create") + std::string(CAM_DIR) + std::string(" directory!"));
    }
  }
  
  InitGlobalHostLogin();
  InitSecurityAttr();
  InitializeHook();
  InitializeMemory();
}

void LIB::Deinitialize(){
  DeinitializeHook();
  WSACleanup();
}

bool LIB::onAsciiCharacter(WORD character){
  if(g_gui->onKeyboardEvent(EVENT_KEY_DOWN, KEYBOARD_STATE_ASCII, character)){
    return true;
  }
  
  return false;
}

void LIB::checkCursorPos(){
  static POINT prevRealPosition;
  static POINT lastRealPosition;
  static POINT prevCallPosition;
  static POINT lastCallPosition;
  
  POINT currentPosition;
  GetCursorPos(&currentPosition);
  if(ScreenToClient(m_hWnd, &currentPosition)){
    prevRealPosition = lastRealPosition;
    lastRealPosition = currentPosition;
    prevCallPosition = lastCallPosition;
    lastCallPosition = g_lastMousePosition;
    
    static int sameCursorPosition = 0;
    if(prevRealPosition.x == lastRealPosition.x && prevRealPosition.y == lastRealPosition.y){
      sameCursorPosition++;
    } else {
      sameCursorPosition = 0;
    }
    
    if(sameCursorPosition > 6 && (lastCallPosition.x != lastRealPosition.x && lastCallPosition.y != lastRealPosition.y) && (prevCallPosition.x != lastCallPosition.x && prevCallPosition.y != lastCallPosition.y)){
      // we didn't move our cursor, but yet the position received by callback changes
      m_validCursor = false;
    }
  }
}

void LIB::FirstTick(){
  /**
    This function will be called only once after the first tick
  */

  // UNCOMMENT TO ENABLE UPDATER
  // Lets create update window
  //m_updateDialog = new GUIUpdateWindow();
  
  // UNCOMMENT TO ENABLE UPDATER  
  // Lets make it as dialog
  //g_gui->setDialog(m_updateDialog);
  
  //GUITestDialog* dialog = new GUITestDialog();
  //g_gui->setDialog(dialog);
  
  // Load configuration file for the advanced options
  m_optionsDialog->loadConfig();
  
  // Everything has been initialized, this function won't be called anymore
  m_firstFrame = true;
}

void LIB::onConnectEvent(const struct sockaddr* address){
  sockaddr_in* addr_in = (sockaddr_in*)address;
  
  if(pinger){
    m_pingHost = ipbintostr(addr_in->sin_addr.s_addr);
    pinger->setHost(g_dll.m_pingHost);
  } else {
    m_pingHost = ipbintostr(addr_in->sin_addr.s_addr);
  }
  
  // Reset the first experience
  m_firstExperience = false;
}

void LIB::onLogoutEvent(){
  m_online = false;
  m_buffer = NULL;
  
  /* Dialogs actions */
  m_tradeDialog->stop();
  
  if(cam->isRecorderEnabled()){
    flushCamRecording(true);
  }
}

void LIB::InitializeMemory(){
  static int m_chunkLength = 28;
  
  /*char* */m_chunk = new char[m_chunkLength];
  ZeroMemory(m_chunk, m_chunkLength);
  
  // mana:
  // 0044B978  |. A3 2C685C00    MOV DWORD PTR DS:[5C682C],EAX
  // 0044BA20  /$ A1 2C685C00    MOV EAX,DWORD PTR DS:[5C682C]
  // 00450A84   . 891D 2C685C00  MOV DWORD PTR DS:[5C682C],EBX
  
  AsmDword(0x44B978 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_MANA);
  AsmDword(0x44BA20 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_MANA);
  AsmDword(0x450A84 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_MANA);
  
  // health:
  // 0044B938  |. A3 48685C00    MOV DWORD PTR DS:[5C6848],EAX
  // 0044B9D0  /$ A1 48685C00    MOV EAX,DWORD PTR DS:[5C6848]
  // 00450A66   . 891D 48685C00  MOV DWORD PTR DS:[5C6848],EBX
  // 00450DB8   . A1 48685C00    MOV EAX,DWORD PTR DS:[5C6848]
  
  AsmDword(0x44B938 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_HEALTH);
  AsmDword(0x44B9D0 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_HEALTH);
  AsmDword(0x450A66 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_HEALTH);
  AsmDword(0x450DB8 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_HEALTH);
  
  // PLAYER_ID:
  // 0044B924  |. A3 4C685C00    MOV DWORD PTR DS:[5C684C],EAX
  // 0044B9C0  /$ A1 4C685C00    MOV EAX,DWORD PTR DS:[5C684C]
  // 0044BBF2  |. 3B05 4C685C00  CMP EAX,DWORD PTR DS:[5C684C]
  // 0044BC12  |. 3B05 4C685C00  CMP EAX,DWORD PTR DS:[5C684C]
  // 0044BC32  |. 3B05 4C685C00  CMP EAX,DWORD PTR DS:[5C684C]
  // 0044BCD7  |. 3B0D 4C685C00  CMP ECX,DWORD PTR DS:[5C684C]  
  // 0044D6D0  /$ A1 4C685C00    MOV EAX,DWORD PTR DS:[5C684C]
  // 0044D6F0  /$ A1 4C685C00    MOV EAX,DWORD PTR DS:[5C684C]
  // 0044D720  /$ A1 4C685C00    MOV EAX,DWORD PTR DS:[5C684C]  
  // 0044D88F  |> 8B0D 4C685C00  MOV ECX,DWORD PTR DS:[5C684C]
  // 00450A60   . 891D 4C685C00  MOV DWORD PTR DS:[5C684C],EBX
  // 00450DC5   . 8B15 4C685C00  MOV EDX,DWORD PTR DS:[5C684C]
  // 0045114B   . 8B15 4C685C00  MOV EDX,DWORD PTR DS:[5C684C]  
  // 004516FE  |. A1 4C685C00    MOV EAX,DWORD PTR DS:[5C684C]
  // 004518CE  |. A1 4C685C00    MOV EAX,DWORD PTR DS:[5C684C]
  // 004519B6   . A1 4C685C00    MOV EAX,DWORD PTR DS:[5C684C]  
  // 00451A59   . 8B15 4C685C00  MOV EDX,DWORD PTR DS:[5C684C]
  // 00451C0D   . 8B0D 4C685C00  MOV ECX,DWORD PTR DS:[5C684C]    
  // 00451ECD  |. A1 4C685C00    MOV EAX,DWORD PTR DS:[5C684C]
  
  AsmDword(0x44B924 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);
  AsmDword(0x44B9C0 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);  
  AsmDword(0x44BBF2 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);
  AsmDword(0x44BC12 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);
  AsmDword(0x44BC32 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);
  AsmDword(0x44BCD7 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);    
  AsmDword(0x44D6D0 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);
  AsmDword(0x44D6F0 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);
  AsmDword(0x44D720 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);    
  AsmDword(0x44D88F + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);
  AsmDword(0x450A60 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);
  AsmDword(0x450DC5 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);
  AsmDword(0x45114B + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);    
  AsmDword(0x4516FE + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);
  AsmDword(0x4518CE + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);
  AsmDword(0x4519B6 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);      
  AsmDword(0x451A59 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);
  AsmDword(0x451C0D + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);      
  AsmDword(0x451ECD + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_ID);
  
  // PLAYER_U:
  /*
  004B6494  MOV DWORD PTR DS:[5D16E4],EAX
  004B6516  MOV EAX,DWORD PTR DS:[5D16E4]
  004BDF92  MOV ECX,DWORD PTR DS:[5D16E4]
  004BE9E1  MOV EAX,DWORD PTR DS:[5D16E4]
  004BF6A6  MOV DWORD PTR DS:[5D16E4],ESI
  */
  
  AsmDword(0x4B6494 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_U);
  AsmDword(0x4B6516 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_U);
  AsmDword(0x4BDF92 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_U);
  AsmDword(0x4BE9E1 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_U);
  AsmDword(0x4BF6A6 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_U);
  
  
  // PLAYER_Z:
  /*
  004B08B7   MOV DWORD PTR DS:[5D16E8],EDX
  004B08DB   MOV EDX,DWORD PTR DS:[5D16E8]             DS:[005D16E8]=00000000
  004B08F0   MOV ECX,DWORD PTR DS:[5D16E8]             DS:[005D16E8]=00000000
  004B0910   MOV EAX,DWORD PTR DS:[5D16E8]             [005D16E8]=00000000
  004B0951   MOV ECX,DWORD PTR DS:[5D16E8]             DS:[005D16E8]=00000000
  004B0960   MOV EAX,DWORD PTR DS:[5D16E8]             [005D16E8]=00000000
  004B09A1   MOV ESI,DWORD PTR DS:[5D16E8]             DS:[005D16E8]=00000000
  004BB1CF   MOV ECX,DWORD PTR DS:[5D16E8]             DS:[005D16E8]=00000000
  004BC2DA   MOV ECX,DWORD PTR DS:[5D16E8]             DS:[005D16E8]=00000000
  004BE1CC   MOV ECX,DWORD PTR DS:[5D16E8]             DS:[005D16E8]=00000000
  004BEBAE   MOV ECX,DWORD PTR DS:[5D16E8]             DS:[005D16E8]=00000000
  004BEC2F   MOV EAX,DWORD PTR DS:[5D16E8]             [005D16E8]=00000000
  004BF6A0   MOV DWORD PTR DS:[5D16E8],EDI
  */
  
  AsmDword(0x4B08B7 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Z);
  AsmDword(0x4B08DB + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Z);
  AsmDword(0x4B08F0 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Z);
  AsmDword(0x4B0910 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Z);
  AsmDword(0x4B0951 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Z);
  AsmDword(0x4B0960 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Z);
  AsmDword(0x4B09A1 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Z);
  AsmDword(0x4BB1CF + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Z);
  AsmDword(0x4BC2DA + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Z);
  AsmDword(0x4BE1CC + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Z);
  AsmDword(0x4BEBAE + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Z);
  AsmDword(0x4BEC2F + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Z);
  AsmDword(0x4BF6A0 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Z);
  
  // PLAYER_Y:
  /*
  004B08B1   MOV DWORD PTR DS:[5D16EC],ECX
  004B08D0   MOV EAX,DWORD PTR DS:[5D16EC]             [005D16EC]=00000000
  004B0941   MOV ECX,DWORD PTR DS:[5D16EC]             DS:[005D16EC]=00000000
  004B0988   MOV EDI,DWORD PTR DS:[5D16EC]             DS:[005D16EC]=00000000
  004BC300   SUB EDX,DWORD PTR DS:[5D16EC]             DS:[005D16EC]=00000000
  004BD584   MOV EAX,DWORD PTR DS:[5D16EC]             [005D16EC]=00000000
  004BD5A6   MOV DWORD PTR DS:[5D16EC],EAX
  004BEBFB   MOV EDX,DWORD PTR DS:[5D16EC]             DS:[005D16EC]=00000000
  004BF69A   MOV DWORD PTR DS:[5D16EC],EDI
  */
  
  AsmDword(0x4B08B1 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Y);
  AsmDword(0x4B08D0 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Y);
  AsmDword(0x4B0941 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Y);
  AsmDword(0x4B0988 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Y);
  AsmDword(0x4BC300 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Y);
  AsmDword(0x4BD584 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Y);
  AsmDword(0x4BD5A6 + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Y);
  AsmDword(0x4BEBFB + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Y);
  AsmDword(0x4BF69A + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_Y);
  
  // PLAYER_X: 
  /*
  004B08AC   MOV DWORD PTR DS:[5D16F0],EAX
  004B08C0   MOV ECX,DWORD PTR DS:[5D16F0]             DS:[005D16F0]=00000000
  004B0930   MOV ECX,DWORD PTR DS:[5D16F0]             DS:[005D16F0]=00000000
  004B096D   MOV EDI,DWORD PTR DS:[5D16F0]             DS:[005D16F0]=00000000
  004BC2EE   SUB EDX,DWORD PTR DS:[5D16F0]             DS:[005D16F0]=00000000
  004BD57E   MOV ECX,DWORD PTR DS:[5D16F0]             DS:[005D16F0]=00000000
  004BD5A0   MOV DWORD PTR DS:[5D16F0],ECX
  004BEBEF   MOV EDX,DWORD PTR DS:[5D16F0]             DS:[005D16F0]=00000000
  004BF694   MOV DWORD PTR DS:[5D16F0],EDI
  */  
  
  AsmDword(0x4B08AC + 1, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_X);
  AsmDword(0x4B08C0 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_X);
  AsmDword(0x4B0930 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_X);
  AsmDword(0x4B096D + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_X);
  AsmDword(0x4BC2EE + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_X);
  AsmDword(0x4BD57E + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_X);
  AsmDword(0x4BD5A0 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_X);
  AsmDword(0x4BEBEF + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_X);
  AsmDword(0x4BF694 + 2, (DWORD)m_chunk + CHUNK_OFFSET_PLAYER_X);
}

void LIB::InitHostLogin(std::string host, uint16_t port){
  strcpy((char*)MemLoginServer1, host.c_str());
  strcpy((char*)MemLoginServer2, host.c_str());
  strcpy((char*)MemLoginServer3, host.c_str());
  strcpy((char*)MemLoginServer4, host.c_str());
  
  if(iequals(host, GLOBAL_HOST)){
    strcpy((char*)MemTestLoginServer, GLOBAL_HOST_PROXY);
  } else {
    strcpy((char*)MemTestLoginServer, host.c_str());
  }
  
  *(uint16_t*)MemPortLoginServer1 = port;
  *(uint16_t*)MemPortLoginServer2 = port;
  *(uint16_t*)MemPortLoginServer3 = port;
  *(uint16_t*)MemPortLoginServer4 = port;
  *(uint16_t*)MemPortLoginServer5 = port;
}

void LIB::InitLocalHostLogin(){
  InitHostLogin(CAM_PLAYER_HOST, CAM_PLAYER_PORT);
}

void LIB::InitGlobalHostLogin(){
  /* Tibia.exe 7.72 port addr = CA1FB */
  //InitHostLogin(GLOBAL_HOST, GLOBAL_PORT);
  
  strcpy((char*)MemLoginServer1, GLOBAL_LOGIN1);
  strcpy((char*)MemLoginServer2, GLOBAL_LOGIN2);
  strcpy((char*)MemLoginServer3, GLOBAL_LOGIN3);
  strcpy((char*)MemLoginServer4, GLOBAL_LOGIN4);
  
  strcpy((char*)MemTestLoginServer, GLOBAL_HOST_PROXY);
  
  *(uint16_t*)MemPortLoginServer1 = GLOBAL_PORT;
  *(uint16_t*)MemPortLoginServer2 = GLOBAL_PORT;
  *(uint16_t*)MemPortLoginServer3 = GLOBAL_PORT;
  *(uint16_t*)MemPortLoginServer4 = GLOBAL_PORT;
  *(uint16_t*)MemPortLoginServer5 = GLOBAL_PORT;
}

void LIB::RequestSharedExperience(){
  g_dll.m_sharedExperience = !g_dll.m_sharedExperience;
  
  g_dll.m_networkMessage = new NetworkMessage(NetworkMessage::XTEA);
  g_dll.m_networkMessage->AddByte(0xA8);
  g_dll.m_networkMessage->AddByte(g_dll.m_sharedExperience);
  g_dll.SendXTEANetworkMessage(g_dll.m_networkMessage);
}

void LIB::RequestOutfitDialog(){
  g_dll.m_networkMessage = new NetworkMessage(NetworkMessage::XTEA);
  g_dll.m_networkMessage->AddByte(0xD2);
  g_dll.SendXTEANetworkMessage(g_dll.m_networkMessage);
}

void LIB::RequestQuestLine(uint16_t questId){
  g_dll.m_networkMessage = new NetworkMessage(NetworkMessage::XTEA);
  g_dll.m_networkMessage->AddByte(0xF1);
  g_dll.m_networkMessage->AddU16(questId);
  g_dll.SendXTEANetworkMessage(g_dll.m_networkMessage);
}

void LIB::SendXTEANetworkMessage(NetworkMessage* message){
  message->writeMessageLength();
  XTEA_encrypt(*message, (uint32_t*)XTEA_KEY_ADDRESS);
  
  char* m_messageBuffer = message->getBuffer();
  
  int transmitted = 0;
  int bytestosend = *(uint16_t*)m_messageBuffer + NetworkMessage::header_length;

  // encrypt with TSA
  /* uint32_t* m_bodyBuffer = (uint32_t*)(m_messageBuffer + NetworkMessage::header_length);
  for(int i = 0; i < (bytestosend - NetworkMessage::header_length)/4; i++){
    m_bodyBuffer[i] ^= *(uint32_t*)PLAYER_ACCOUNT_NUMBER ^ 88;
  } */
 
  while(transmitted != bytestosend){
    int ret = send(g_dll.m_socket, m_messageBuffer + transmitted, bytestosend - transmitted, 0);
    if(ret == -1){
      break;
    }
    
    transmitted = transmitted + ret;
  }
  delete message;
}

void LIB::SendRSANetworkMessage(NetworkMessage* message){

}

void LIB::SetExperienceText(bool Display, uint32_t Experience /*= 0*/){
  if(!Display){
    if(m_displayExperience){
      SetWindowText(m_hWnd, (WINDOW_TITLE));
    }
  } else {
    uint32_t experiencePerMinute = (float)(Experience - m_beginExperience) / (float)((GetTickCount() - m_beginExpTime) / 1000.0f / 60.0f);
    
    std::stringstream m_title;
    m_title << WINDOW_TITLE << " - You are now gaining " << (experiencePerMinute * 60) << " experience points per hour";
  

    SetWindowText(m_hWnd, ((char*)m_title.str().c_str()));
  }
}

DWORD LIB::HookAsmCall(DWORD dwAddress, DWORD dwFunction, DWORD NOPs /* = 0 */){
  DWORD dwOldProtect, dwNewProtect, dwOldCall, dwNewCall;
  HANDLE proc = GetCurrentProcess();
  
  VirtualProtectEx(proc, (LPVOID)(dwAddress), 5 + NOPs, PAGE_READWRITE, &dwOldProtect);
  
  dwNewCall = dwFunction - dwAddress - 5;
  dwOldCall = *(uint32_t*)(dwAddress + 1);  
  
  *(uint8_t*)(dwAddress) = 0xE8;
  *(uint32_t*)(dwAddress + 1) = dwNewCall;
  // 0xE8 AABBCCDD - asm CALL func
  
  for(int i = 0; i < NOPs; i++){
    *(uint8_t*)(dwAddress + 5 + i) = 0x90; // NOP
  }
  
  VirtualProtectEx(proc, (LPVOID)(dwAddress), 5 + NOPs, dwOldProtect, &dwNewProtect);
  
  return dwOldCall;
}

void LIB::AsmNop(DWORD dwAddress, DWORD dwNops){
  DWORD dwOldProtect, dwNewProtect;
  HANDLE proc = GetCurrentProcess();
  
  VirtualProtectEx(proc, (LPVOID)(dwAddress), dwNops, PAGE_READWRITE, &dwOldProtect);
  
  for(int i = 0; i < dwNops; i++){
    *(uint8_t*)(dwAddress + i) = 0x90; // NOP
  }
  
  VirtualProtectEx(proc, (LPVOID)(dwAddress), dwNops, dwOldProtect, &dwNewProtect);
}

void LIB::AsmDword(DWORD dwAddress, DWORD dwFunction){
  DWORD dwOldProtect, dwNewProtect, dwOldCall, dwNewCall;
  HANDLE proc = GetCurrentProcess();
  VirtualProtectEx(proc, (LPVOID)(dwAddress), 4, PAGE_READWRITE, &dwOldProtect);  
  dwNewCall = dwFunction;
  dwOldCall = *(uint32_t*)(dwAddress);  
  *(uint32_t*)(dwAddress) = dwNewCall;
  VirtualProtectEx(proc, (LPVOID)(dwAddress), 4, dwOldProtect, &dwNewProtect);  
  //return dwOldCall;
}

void LIB::InitializeHook(){
  DWORD dwOldProtect, dwNewProtect, funcAddress, origAddress;
  
  funcAddress = (DWORD)&HookedConnect;
  origAddress = (DWORD)((int*)IAT_CONNECT_FUNCTION_ADDRESS); //ws2_32 connect in Tibia.exe IAT
  VirtualProtect((LPVOID)origAddress, 4, PAGE_READWRITE, &dwOldProtect);
  memcpy((LPVOID)origAddress, &funcAddress, 4);
  VirtualProtect((LPVOID)origAddress, 4, dwOldProtect, &dwNewProtect);    
  
  funcAddress = (DWORD)&HookedSend;
  origAddress = (DWORD)((int*)IAT_SEND_FUNCTION_ADDRESS); //ws2_32 send in Tibia.exe IAT
  VirtualProtect((LPVOID)origAddress, 4, PAGE_READWRITE, &dwOldProtect);
  memcpy((LPVOID)origAddress, &funcAddress, 4);
  VirtualProtect((LPVOID)origAddress, 4, dwOldProtect, &dwNewProtect);     
  
  funcAddress = (DWORD)&HookedRecv;
  origAddress = (DWORD)((int*)IAT_RECV_FUNCTION_ADDRESS); //ws2_32 recv in Tibia.exe IAT
  VirtualProtect((LPVOID)origAddress, 4, PAGE_READWRITE, &dwOldProtect);
  memcpy((LPVOID)origAddress, &funcAddress, 4);
  VirtualProtect((LPVOID)origAddress, 4, dwOldProtect, &dwNewProtect);   
  
  funcAddress = (DWORD)&HookedCreateWindowEx;
  origAddress = (DWORD)((int*)IAT_CREATEWINDOWEX_FUNCTION_ADDRESS); //USER32.CreateWindowExA
  VirtualProtect((LPVOID)origAddress, 4, PAGE_READWRITE, &dwOldProtect);
  memcpy((LPVOID)origAddress, &funcAddress, 4);
  VirtualProtect((LPVOID)origAddress, 4, dwOldProtect, &dwNewProtect);
  
  // fix the wrong func address:
  HookAsmCall(DRAW_SKIN_MENU_GAME_FUNCTION_ADDRESS, DRAW_SKIN_FUNCTION_ADDRESS);
  
  HookAsmCall(PING_CALL_ADDRESS, (DWORD)&HookedPing);
  
  HookAsmCall(PRINT_FPS_FUNCTION_ADDRESS, (DWORD)&HookedPrintFps);
  // 004494F9   . 83C4 24        ADD ESP,24
  // The asm will pop the stack from function arguments
  
  HookAsmCall(PRINT_CAPACITY_FUNCTION_ADDRESS, (DWORD)&HookedPrintCapacity);
  // 00428F2D  |. 83C4 24        ADD ESP,24
  // The asm will pop the stack from function arguments
  
  // Both functions have to be checked for stack validation!
  HookAsmCall(MINIMISE_INVENTORY_REPLACEMENT_ADDRESS, (DWORD)&HookedMinInventory);
  HookAsmCall(MAXIMIZE_INVENTORY_REPLACEMENT_ADDRESS, (DWORD)&HookedMaxInventory);
  
  // clear the code
  AsmNop(GUI_CLICK_EVENT_CODECAVE_CALL_ADDRESS, 6);
  
  // new code - check for stack validation!
  HookAsmCall(GUI_CLICK_EVENT_CODECAVE_CALL_ADDRESS, (DWORD)&HookedOnPushEvent);  
  
  //commented
  //HookAsmCall(ADD_CONTEXT_MENU_EX_CALL_SET_OUTFIT_FUNCTION_ADDRESS, (DWORD)&HookedAddSetOutfitContextMenu);
  
  HookAsmCall(0x0044B089, (DWORD)&HookedParsePacketSwitch);
  
  // new code - check for stack validation!
  HookAsmCall(0x004C8C6C, (DWORD)&HookedDrawCreatureName);
  
  // STDCALL
  HookAsmCall(0x0043729A, (DWORD)&HookedGetIconSkin);
  
  // new code - check for stack validation!
  HookAsmCall(0x004A9B30, (DWORD)&HookedTimer);
  
  //HookAsmCall(0x42B8F5, (DWORD)&HookedMenuButton);
  HookAsmCall(SET_EXPERIENCE_CALL_ADDRESS, (DWORD)&HookedSetExperience);
  HookAsmCall(PUSH_LETTER_CALL_ADDRESS, (DWORD)&HookedPushLetter);
  
  //00449458   . 0F84 9E000000  JE Tibia.004494FC
  // printfps jump
  AsmNop(PRINT_FPS_JUMP_ADDRESS, 6);
}

void LIB::DeinitializeHook(){

}

bool LIB::CreateDACL(SECURITY_ATTRIBUTES* pSA){
  const TCHAR * szSD =TEXT("D:")             // Discretionary ACL
            TEXT("(D;OICI;GA;;;BG)")    // Deny all access to built-in guests
            TEXT("(D;OICI;GA;;;AN)")    // Deny all access to anonymous logon
            TEXT("(D;OICI;GA;;;AU)")    // Deny all access to authenticated users
            TEXT("(D;OICI;GA;;;BA)");   // Deny full control to administrators
//  return true;
  return ConvertStringSecurityDescriptorToSecurityDescriptor(szSD, 1, (PSECURITY_DESCRIPTOR*)&(pSA->lpSecurityDescriptor), NULL);
}

bool LIB::InitSecurityAttr(){
  SECURITY_ATTRIBUTES sa;
  
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = FALSE;

  CreateDACL(&sa);
  if(!SetKernelObjectSecurity(m_currentProcess, DACL_SECURITY_INFORMATION, (PSECURITY_DESCRIPTOR)sa.lpSecurityDescriptor)){
    m_error = std::string("Initializing security attributes failed!");
  }
}

extern "C" BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved){
  switch(reason){
    case DLL_PROCESS_ATTACH: {
        g_dll.Initialize();    
        break;
      }

    case DLL_PROCESS_DETACH: {
        g_dll.Deinitialize();
        break;
      }

    case DLL_THREAD_ATTACH: {
        break;
      }

    case DLL_THREAD_DETACH: {
        break;
      }
      
    default: break;
  }

  return TRUE;
}
