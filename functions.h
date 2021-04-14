#ifndef __FUNCTIONSH__
#define __FUNCTIONSH__

#include <iostream>
#include <windows.h>
#include <stdint.h>

#include "const.h"
#include "structures.h"
#include "uitibia.h"

typedef void _Error(const char* lpTitle, const char* lpText);
typedef void _PushLetter(int Letter);
typedef void _SetExperience(int Experience, int Level, int U3);
typedef int _MinimizeInventory(int u1, int u2);
typedef int _MaximizeInventory(int u1, int u2);
typedef int _TextMetric(int font, char* text, int length);
typedef uint32_t _GetItemProperty(uint32_t itemid, uint32_t flag);
typedef void* _AddContextMenu(int id, const char* text, const char* hint);
typedef Creature_t* _GetCreatureEntry(uint32_t id);
typedef void* _GetPainter();
typedef void _SetUseObject(uint32_t objectId, uint32_t u1, uint32_t u2, uint32_t u3, uint32_t x, uint32_t y, uint32_t z, uint32_t use);
typedef void _Idle();

typedef void _EnterCriticalSection(int section);
typedef void _LeaveCriticalSection(int section);

/* Dialogs */
typedef Tibia::GUIDialog* _CreateTextDialog(const char* caption, const char* text);
typedef Tibia::GUIDialog* _CreateAcceptDialog(const char* caption, const char* text, uint32_t callback, uint32_t u1, uint32_t u2);
typedef Tibia::GUIDialog* _CreateMoveObjectDialog();
typedef Tibia::GUIDialog* _CreateOptionsDialog();
typedef Tibia::GUIDialog* _CreateHelpDialog(void* u1);
typedef void* _CreateCharacterListDialog(uint32_t u1);

/* Network */
typedef uint32_t _NetworkGetU32();
typedef uint16_t _NetworkGetU16();
typedef uint8_t _NetworkGetU8();
typedef char* _NetworkGetString();

/* Basic functions */
namespace Tibia {
	extern Tibia::GUI* GetGUIPointer();
	extern uint32_t GetPlayerInfo(playerInfo_t info);
	extern bool HasDialog();
	extern void SetCurrentDialog(GUIDialog* dialog);
	extern void FatalError(const char* error);
	extern void Exit();
	extern bool CanLogout();
	extern bool IsOnline();
	
	/* Critical sections */
	extern _EnterCriticalSection *EnterCriticalSection;
	extern _LeaveCriticalSection *LeaveCriticalSection;

	/* Tibia functions */
	extern _Error *Error;
	extern _TextMetric *TextMetric;

	extern _GetItemProperty * GetItemProperty;

	extern _MinimizeInventory *MinimizeInventory;
	extern _MaximizeInventory *MaximizeInventory;
	extern _SetExperience *SetExperience;
	extern _PushLetter *PushLetter;
	extern _AddContextMenu *AddContextMenu;
	extern _GetCreatureEntry * GetCreatureEntry;

	extern _CreateTextDialog *CreateTextDialog;
	extern _CreateAcceptDialog *CreateAcceptDialog;
	extern _CreateCharacterListDialog *CreateCharacterListDialog;
	extern _CreateMoveObjectDialog* CreateMoveObjectDialog;
	extern _CreateOptionsDialog *CreateOptionsDialog;
	extern _CreateHelpDialog *CreateHelpDialog;
	extern _GetPainter *GetPainter;
	extern _SetUseObject *SetUseObject;
	extern _Idle *Idle;

	extern _NetworkGetU32 *NetworkGetU32;
	extern _NetworkGetU16 *NetworkGetU16;
	extern _NetworkGetU8 *NetworkGetU8;
	extern _NetworkGetString *NetworkGetString;
}

#endif
