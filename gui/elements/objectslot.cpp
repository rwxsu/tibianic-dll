#include "objectslot.h"
#include "main.h"

#include <windows.h>

Object::Object(int x, int y, int objectId, int objectData /*= 0*/, int size /*= 32*/) : GUIElement(x, y, size, size) {
  m_objectId = objectId;
  m_objectData = objectData;
  m_objectSize = size;
  m_objectOffset = 0;
}
    
void Object::setObjectId(int objectId){
  m_objectId = objectId;
}

void Object::setObjectData(int objectData){
  m_objectData = objectData;
}
		
void Object::draw(int surface){
  /* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
  
  Painter::drawItem(surface, absolute->x + m_objectOffset, absolute->y + m_objectOffset, m_objectSize, m_objectId, m_objectData, 0, 0, 0, 0, 0, 0, 0xFFFF, 0xFFFF, FONT_NORMAL_OUTLINED, 191, 191, 191, 2, 0);
}

ObjectSlot::ObjectSlot(int x, int y, int objectId, int objectData /*= 0*/, int size /*= 32*/) : Object(x, y, objectId, objectData, size) {
  GUIElement::setWidth(GUIElement::getWidth() + 2);
  GUIElement::setHeight(GUIElement::getHeight() + 2);
  
  m_objectOffset = 1;
}
    
void ObjectSlot::draw(int surface){
  /* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
  
  /* Draw background */
  Painter::drawSkin(surface, absolute->x, absolute->y, 34, 34, 102, 0, 0);
  
  /* Draw object itself */
  Object::draw(surface);
}

ArrowSlot::ArrowSlot(int x, int y, int emptyId, int objectId, int objectData) : GUIElement (x, y, 34, 34) {
	m_emptyId = emptyId;
	m_objectId = objectId;
	m_objectData = objectData;
	
	m_state = USE_NONE;
}

bool ArrowSlot::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	if(isPointInRectangle(x, y, m_position.x, m_position.y, 34, 34)){
		switch(event){
			case EVENT_BUTTON: {
				if(leftButtonDown){
					if(*(uint32_t*)(PLAYER_SLOT_AMMO_ADDRESS)){
						m_state = USE_LEFT;
						///*
						*(uint32_t*)0x71C5E8 = USE_LEFT;
						*(uint32_t*)0x71C5C4 = 0xFFFF;
						*(uint32_t*)0x71C5C0 = 10;
						*(uint32_t*)0x71C5Bc = 0;
						*(uint32_t*)0x71C624 = *(uint32_t*)(PLAYER_SLOT_AMMO_ADDRESS); // item
						*(uint32_t*)(0x71C624 + 4) = *(uint32_t*)(PLAYER_SLOT_AMMO_ADDRESS + 4); // amount
						//*/
						
						//SetUseObject(*(uint32_t*)(PLAYER_SLOT_AMMO_ADDRESS), 0, 0, 0, 0xFFFF, 0x0A, 0x00, 0x0A);
						
						return false;
					} else {
						m_state = USE_NONE;
						return true;
					}
				} else if(m_state != USE_NONE){
					Use_t state = m_state;
					
					m_state = USE_NONE;
					*(uint32_t*)0x71C5E8 = USE_NONE;
					
					
					if(state == USE_LEFT){
						if(g_lastControl){ // use item
							if(Tibia::GetItemProperty(*(uint32_t*)(PLAYER_SLOT_AMMO_ADDRESS), 0x07)){ // use with
								/*
								*(uint32_t*)0x71C5E8 = USE_USING;
								*(uint32_t*)0x71C5C4 = 0xFFFF;
								*(uint32_t*)0x71C5C0 = 10;
								*(uint32_t*)0x71C5Bc = 0;
								*(uint32_t*)0x71C624 = *(uint32_t*)(PLAYER_SLOT_AMMO_ADDRESS); // item
								*(uint32_t*)(0x71C624 + 4) = *(uint32_t*)(PLAYER_SLOT_AMMO_ADDRESS + 4); // amount
								
								*(uint32_t*)0x71C5B8 = 0xFFFF; // x
								*(uint32_t*)0x71C5B4 = 10;// y
								*(uint32_t*)0x71C5B0 = 0;// z
								
								// source Object:
								*(uint32_t*)0x71C598 = 0xFFFF;
								*(uint32_t*)0x71C594 = 10;
								*(uint32_t*)0x71C590 = 0;
								
								*(uint32_t*)0x71C640 = 0xFFFF; // x
								*(uint32_t*)0x71C644 = 10; // y
								*(uint32_t*)0x71C63C = 0; // z
								
								*(uint32_t*)0x71C59C = 0; // ??
								
								
								*(uint32_t*)0x0071C63C = *(uint32_t*)(PLAYER_SLOT_AMMO_ADDRESS); // item
								//
								SetUseObject(*(uint32_t*)(PLAYER_SLOT_AMMO_ADDRESS), 0, 0, 0, 0xFFFF, 0x0A, 0x00, 0x0A);
								return true;
								*/
								return false;
							} else {
								g_dll.m_networkMessage = new NetworkMessage(NetworkMessage::XTEA);
								g_dll.m_networkMessage->AddByte(0x82);
								g_dll.m_networkMessage->AddPosition(Position(0xFFFF, 10, 0));
								g_dll.m_networkMessage->AddU16(*(uint32_t*)(PLAYER_SLOT_AMMO_ADDRESS)); // spriteId
								g_dll.m_networkMessage->AddByte(0); // from stack pos
								g_dll.m_networkMessage->AddByte(0); // container index
								g_dll.SendXTEANetworkMessage(g_dll.m_networkMessage);
								return false;
							}
						} else if(GetKeyState(VK_SHIFT) & KEY_DOWN){
							g_dll.m_networkMessage = new NetworkMessage(NetworkMessage::XTEA);
							g_dll.m_networkMessage->AddByte(0x8C);
							g_dll.m_networkMessage->AddPosition(Position(0xFFFF, 10, 0));
							g_dll.m_networkMessage->AddU16(*(uint32_t*)(0x71C624)); // spriteId
							g_dll.m_networkMessage->AddByte(0); // from stack pos
							g_dll.SendXTEANetworkMessage(g_dll.m_networkMessage);
							return false;
						}
					}

					// check somehow if it wasnt a move or?					

					if(*(uint32_t*)(0x71C624 + 4) > 1 && !g_lastControl){ // if more than 1 and no control key pushed
						*(uint32_t*)0x71C5B8 = 0xFFFF; // x
						*(uint32_t*)0x71C5B4 = 10;// y
						*(uint32_t*)0x71C5B0 = 0;// z
						
						Tibia::GUIDialog* dialog = Tibia::CreateMoveObjectDialog();
						Tibia::SetCurrentDialog(dialog);
					} else {
						g_dll.m_networkMessage = new NetworkMessage(NetworkMessage::XTEA);
						g_dll.m_networkMessage->AddByte(0x78);
						g_dll.m_networkMessage->AddPosition(Position(*(uint32_t*)0x71C5C4, *(uint32_t*)0x71C5C0, *(uint32_t*)0x71C5BC)); // topos
						g_dll.m_networkMessage->AddU16(*(uint32_t*)(0x71C624)); // spriteId
						g_dll.m_networkMessage->AddByte(0); // from stack pos
						g_dll.m_networkMessage->AddPosition(Position(0xFFFF, 10, 0)); // to pos
						g_dll.m_networkMessage->AddByte(*(uint32_t*)(0x71C624 + 4)); // amount, usually 1
						g_dll.SendXTEANetworkMessage(g_dll.m_networkMessage);
					}
					
					return false;
				}
				
				break;
			}
			
			case EVENT_MOVEMENT: {
				if(m_state == USE_LEFT){
					m_state = USE_DRAG;
					*(uint32_t*)0x71C5E8 = USE_DRAG;				
					return true;
				} else if(*(uint32_t*)0x71C5E8 == USE_DRAG){
					m_state = USE_DRAG;
					return true;
				}
				
				break;
			}
			
			default: break;
		}
	} else {
		switch(event){
			case EVENT_BUTTON: {
				if(!leftButtonDown && m_state != USE_NONE){
					m_state = USE_NONE;
					//*(uint32_t*)0x71C5E8 = USE_NONE;
					return false;
				}
				
				break;
			}
			
			case EVENT_MOVEMENT: {
				if(m_state == USE_DRAG){
					m_state = USE_LEFT;
					return true;
				}
				
				break;
			}
			
			default: break;
		}
	}
	
	return false;
}

void ArrowSlot::draw(int surface){
	/* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
	
	Painter::drawSkin(surface, absolute->x, absolute->y, 34, 34, 102, 0, 0);
	
	if(*(uint32_t*)(PLAYER_SLOT_AMMO_ADDRESS)){
		Painter::drawItem(surface, absolute->x + 1, absolute->y + 1, 32, *(uint32_t*)(PLAYER_SLOT_AMMO_ADDRESS), *(uint32_t*)(PLAYER_SLOT_AMMO_ADDRESS + 4), 0, 0, 0, 0, 0, 0, 0xFFFF, 0xFFFF, FONT_NORMAL_OUTLINED, 191, 191, 191, 2, 0);
	} else if(m_emptyId){
		Painter::drawSkin(surface, absolute->x, absolute->y, 34, 34, m_emptyId, 0, 0);
	}
	
	if(m_state == USE_DRAG){
		Painter::drawHollowRectangle(surface, absolute->x, absolute->y, 34, 34, 1, 255, 255, 255);
	}
}
