#ifndef __UITIBIAH__
#define __UITIBIAH__

#include "structures.h"
#include "position.h"

namespace Tibia {
	class GUIItem;
	class GUIHolder;
	class GUI;

	class GUIItem {
		public:
			uint32_t m_vftable;
			uint32_t m_background;
			uint32_t m_partsAmount;
			GUIHolder* m_parent;
			GUIHolder* m_nextItem;
			
			uint32_t m_offsetx; // 0x14
			uint32_t m_offsety; // 0x18
			
			uint32_t m_width; // 0x1C
			uint32_t m_height; // 0x20
			
			/* Static functions */
			static Position* absolutePos(GUIItem* item){
				static Position position;
				
				/* Clear position */
				position.x = 0;
				position.y = 0;
				
				while(item){
					position.x = position.x + item->m_offsetx;
					position.y = position.y + item->m_offsety;
					
					item = (GUIItem*)item->m_parent;
				}
				
				return &position;
			}
	};//__attribute__((packed));

	class GUIHolder : public GUIItem {
		public:
			GUIHolder* m_child; // 0x24 points to firstChild
			GUIHolder* m_clickChild; // 0x28, 4b - points to child window where we clicked by mouse, it's equal to 0 if mouse button isn't pressed
	};//__attribute__((packed));

	class GUI : public GUIHolder {
		public:
			GUIHolder* m_game; //0x30 main game window (map)
			GUIHolder* m_player; //0x34, 4b - points to player window child
			GUIHolder* m_container; //0x38, 4b - points to containers window child
			GUIHolder* m_firstChildOfPlayer; //0x3C, 4b - points to first child of player window child
			GUIHolder* m_chat; //0x40, 4b - points to chat window child
			GUIHolder* m_resize; //0x44, 4b - points to resize bar child
	};//__attribute__((packed));

	class GUIEquipmentSlots : public GUIHolder {
		public:
			uint32_t m_u1;
			uint32_t m_u2;
			uint32_t m_u3;
			uint32_t m_u4;
			
			uint32_t m_u5;
			uint32_t m_u6;
			uint32_t m_u7;
			uint32_t m_u8;
			
			uint32_t m_u9;
			uint32_t m_u10;
			uint32_t m_u11;
			uint32_t m_u12;
			
			uint32_t m_u13;
			uint32_t m_u14;
			uint32_t m_u15;
			uint32_t m_u16;
	};

	class GUIContainer : public GUIHolder { // GUI Window
		public:
			uint32_t m_id;
			/* 
				1 = equipment window
				2 = minimized equipment window ?
				3 = minimap window
				4 = health bar window
				5 = buttons window
				
				6 = Skills window
				7 = Battle window
				9 = Vip window
				64+ = containers
			*/
			
			uint32_t m_u2; // set only when GUI is re-created
			uint16_t m_moveFlags; // 256 = moving window
			uint16_t m_u3;
			Point_t m_clickPos;
			uint32_t m_clickBottomOffset;
			uint32_t m_body;
			uint32_t m_resizeTriangle;
			uint32_t m_u7;
			uint32_t m_u8;
			
			uint32_t m_closeButton;
			uint32_t m_minimizeButton;
			uint32_t m_icon;
			uint32_t m_title;
			
			uint8_t m_minimized;
			uint8_t m_u13;
			uint16_t m_u14;
			
			uint32_t m_u15;
			uint32_t m_u16;
			uint32_t m_u17;
			uint32_t m_u18;
			uint32_t m_u19;
			uint32_t m_u20;
	};

	class GUIDialog : public GUIHolder {
		public:
			uint32_t m_u8; // 0x2C
			uint32_t m_u9; // 0x30
			uint32_t m_u10;
			uint32_t m_u11;
			uint32_t m_u12;
			uint32_t m_u13;
			uint32_t m_u14;
			uint32_t m_u15;
			uint32_t m_u16;
	};//__attribute__((packed));

	class GUIButton : public GUIItem {
		public:
			uint32_t m_skinNormal;
			uint32_t m_skinPush;
			uint32_t m_pushed;
			uint32_t m_u4;
			uint32_t m_u5;
			uint32_t m_u6;
			uint32_t m_u7;
			uint32_t m_active;
			char m_caption[64];
	};//__attribute__((packed));

	class GUISelectBox : public GUIItem {
		public:
			uint32_t m_child;
			uint32_t m_u1;
			uint32_t m_count;
			
			uint32_t m_clipWidth;
			uint32_t m_u3;
			uint32_t m_u4;
			uint32_t m_u5;
			uint32_t m_u6;
			uint32_t m_u7;
			uint32_t m_u8;
			uint32_t m_u9;		
			uint32_t m_u10;
			uint32_t m_u11;
			uint32_t m_u12;
			uint32_t m_u13;
			
	};//__attribute__((packed));

	class GUIText : public GUIItem {
		public:
			uint32_t m_first; // pointer to first line of text
			uint32_t m_text; // pointer to whole text
			uint32_t m_font;
			
			uint32_t m_red;
			uint32_t m_green;
			uint32_t m_blue;
			
			uint32_t m_alignment;
			uint32_t m_lines; // amount of lines
			uint32_t m_u5;
	};//__attribute__((packed));
}

#endif
