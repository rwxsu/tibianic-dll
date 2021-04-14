#ifndef __TRADEWINDOWH__
#define __TRADEWINDOWH__

#include <iostream>
#include <sstream>

#include "guiwindow.h"
#include "const.h"
#include "tools.h"

class GUITradeWindow : public GUIWindow {
	public:
		GUITradeWindow();
		
		/* Override tick for sending messages */
		void tick(uint32_t ticks);
		
		/* Stop all actions */
		void stop();
	protected:
		void invalidate(GUIElement* caller);
	private:
		uint32_t m_ticks;
};

#endif
