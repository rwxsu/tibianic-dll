#ifndef __TESTDIALOGH__
#define __TESTDIALOGH__

#include <iostream>
#include <sstream>

#include "guiwindow.h"
#include "const.h"
#include "tools.h"

class GUITestDialog : public GUIWindow {
	public:
		GUITestDialog();
		
		/* Override tick for sending messages */
		void tick(uint32_t ticks);
};

#endif
