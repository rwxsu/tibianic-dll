#include "functions.h"
#include "const.h"
#include "main.h"

#include <sstream>
#include <windows.h>

namespace Tibia {
	/* Tibia functions */
	_Error *Error = (_Error*)ERROR_FUNCTION_ADDRESS;
	_TextMetric *TextMetric = (_TextMetric*)GET_METRIC_FUNCTION_ADDRESS;
	_MinimizeInventory *MinimizeInventory = (_MinimizeInventory*)MINIMISE_INVENTORY_FUNCTION_ADDRESS;
	_MaximizeInventory *MaximizeInventory = (_MaximizeInventory*)MAXIMIZE_INVENTORY_FUNCTION_ADDRESS;
	_GetItemProperty *GetItemProperty = (_GetItemProperty*)GET_ITEM_PROPERTY_FUNCTION_ADDRESS;
	_SetExperience *SetExperience = (_SetExperience*)SET_EXPERIENCE_FUNCTION_ADDRESS;
	_PushLetter *PushLetter = (_PushLetter*)PUSH_LETTER_FUNCTION_ADDRESS;
	_AddContextMenu *AddContextMenu = (_AddContextMenu*)ADD_CONTEXT_MENU_EX_FUNCTION_ADDRESS;
	_GetCreatureEntry *GetCreatureEntry = (_GetCreatureEntry*)GET_CREATURE_ENTRY_FUNCTION_ADDRESS;
	_CreateTextDialog *CreateTextDialog = (_CreateTextDialog*)WARNING_DIALOG_FUNCTION_ADDRESS;
	_CreateAcceptDialog *CreateAcceptDialog = (_CreateAcceptDialog*)0x004803F0;
	_CreateCharacterListDialog *CreateCharacterListDialog = (_CreateCharacterListDialog*)CHARACTERLIST_DIALOG_FUNCTION_ADDRESS;
	_CreateMoveObjectDialog *CreateMoveObjectDialog = (_CreateMoveObjectDialog*)0x00480CD0;
	_CreateOptionsDialog *CreateOptionsDialog = (_CreateOptionsDialog*)0x00483500;
	_CreateHelpDialog *CreateHelpDialog = (_CreateHelpDialog*)0x00482EA0;
	_GetPainter *GetPainter = (_GetPainter*)GET_PAINTER_FUNCTION_ADDRESS;
	_SetUseObject *SetUseObject = (_SetUseObject*)0x004E70F0;
	_Idle* Idle = (_Idle*)0x004489A0;
	
	_EnterCriticalSection* EnterCriticalSection = (_EnterCriticalSection*)0x0052EC14;
	_LeaveCriticalSection* LeaveCriticalSection = (_LeaveCriticalSection*)0x0052EC77;

	/* Network related functions in Tibia client */
	_NetworkGetU32 *NetworkGetU32 = (_NetworkGetU32*)GET_NETWORK_MESSAGE_U32_ADDRESS;
	_NetworkGetU16 *NetworkGetU16 = (_NetworkGetU16*)GET_NETWORK_MESSAGE_U16_ADDRESS;
	_NetworkGetU8 *NetworkGetU8 = (_NetworkGetU8*)GET_NETWORK_MESSAGE_U8_ADDRESS;
	_NetworkGetString *NetworkGetString = (_NetworkGetString*)GET_NETWORK_MESSAGE_STRING_ADDRESS;

	uint32_t GetPlayerInfo(playerInfo_t info){
		switch(info){
			case PLAYER_INFO_ID: {
				return *(uint32_t*)(g_dll.m_chunk + CHUNK_OFFSET_PLAYER_ID);
			}
			
			case PLAYER_INFO_HEALTH: {
				return *(uint32_t*)(g_dll.m_chunk + CHUNK_OFFSET_PLAYER_HEALTH);
			}
			
			case PLAYER_INFO_MAX_HEALTH: {
				return *(uint32_t*)PLAYER_HP_MAX_ADDRESS;
			}
			
			case PLAYER_INFO_MANA: {
				return *(uint32_t*)(g_dll.m_chunk + CHUNK_OFFSET_PLAYER_MANA);
			}
			
			case PLAYER_INFO_MAX_MANA: {
				return *(uint32_t*)PLAYER_MP_MAX_ADDRESS;
			}
			
			case PLAYER_INFO_TICKS: {
				return *(uint32_t*)PLAYER_TICKS;
			}
			
			case PLAYER_INFO_ICONS: {
				return *(uint32_t*)ICONS_ADDRESS;
			}
			
			default: break;
		}
		
		return 0;
	}

	Tibia::GUI* GetGUIPointer(){
		return (Tibia::GUI*)*(uint32_t*)GET_GUI_POINTER_ADDRESS;
	}

	bool HasDialog(){
		return *(uint32_t*)0x5D16AC != 0;
	}

	void SetCurrentDialog(GUIDialog* dialog){
		if(dialog){
			// Move dialog to center
			dialog->m_offsetx = g_dll.m_screenWidth / 2 - dialog->m_width / 2;
			dialog->m_offsety = g_dll.m_screenHeight / 2 - dialog->m_height / 2;
		}
		
		// Set current dialog
		*(uint32_t*)0x5D16AC = (uint32_t)dialog;
		
		if(dialog){
			// makes the dialog draggable and clickable IDK
			*(uint32_t*)0x71C5E8 = 10;
		} else {
			*(uint32_t*)0x71C5E8 = 0;
		}
	}

	void FatalError(const char* error){
		Error(error, "Tibia Error");
	}
	
	void Exit(){
		ExitProcess(0);
	}

	inline uint32_t getEip(){
		uint32_t eip;
		
		asm(".intel_syntax noprefix");
		asm("mov eax, [ebp+4]");
		asm("mov %0, eax;"
			".att_syntax;"
			: "=r"(eip)
			:);
		
		return eip;
	}

	bool CanLogout(){
		uint8_t flags = *(uint8_t*)ICONS_ADDRESS;
		
		if(flags & FLAG_SWORDS == FLAG_SWORDS){
			return false;
		}
		
		return true;
	}

	bool IsOnline(){
		uint8_t flags = *(uint8_t*)CONNECTION_STATUS_ADDRESS;
		if(flags == 8){
			return true;
		}
		
		return false;
	}
}
