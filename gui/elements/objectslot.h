#ifndef __OBJECTSLOTH__
#define __OBJECTSLOTH__

#include <iostream>

#include "guielement.h"
#include "functions.h"
#include "tools.h"

class Object : public GUIElement {
  public:
    Object(int x, int y, int objectId, int objectData = 0, int size = 32);
    
    /* Set id of the object */
    void setObjectId(int objectId);
    void setObjectData(int objectData);
		
    /* Override draw method */
		virtual void draw(int surface);
  private:
    int m_objectId;
    int m_objectData;
    int m_objectSize;
  protected:
    int m_objectOffset;
};

class ObjectSlot : public Object {
  public:
    ObjectSlot(int x, int y, int objectId, int objectData = 0, int size = 32);
    
    /* Override draw method */
		virtual void draw(int surface);
};

class ArrowSlot : public GUIElement {
	public:
		ArrowSlot(int x, int y, int emptyId, int objectId, int objectData);
		
    /* Override mouse event method */
		virtual bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);
		
    /* Override draw method */
		virtual void draw(int surface);
	private:
		int m_emptyId;
		int m_objectId;
		int m_objectData;
		
		Use_t m_state;
};

#endif
