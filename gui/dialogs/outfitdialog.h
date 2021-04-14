#ifndef __OUTFITDIALOGH__
#define __OUTFITDIALOGH__

#include <iostream>
#include <vector>

#include "const.h"
#include "guiwindow.h"

class GUIOutfitDialog : public GUIWindow {
	public:
		/* Constructor and destructor */
		GUIOutfitDialog();
		
		/* Outfits to choose from */
		void setOutfits(std::vector< std::pair<uint32_t, std::string> > outfits);
		
		/* The outfit of the player */
		void setOutfit(Outfit_t outfit);
		
		/* Static event handlers */
		static void chooseNone(Button* button, void* param);
		static void chooseHead(Button* button, void* param);
		static void chooseBody(Button* button, void* param);
		static void chooseLegs(Button* button, void* param);
		static void chooseFeet(Button* button, void* param);
		
		static void choosePrevious(Button* button, void* param);
		static void chooseNext(Button* button, void* param);
		
		static void chooseAccept(Button* button, void* param);
		
		bool onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown = 0, bool rightButtonDown = 0);
		//bool onKeyboardEvent(KeyboardEvent_t event, keyboardstate_t state, int input){ m_mask++; };
	private:
		/* Override self draw function */
		void drawSelf(int surface);
		
		uint32_t* m_mask;
		Outfit_t m_outfit;
		
		int m_outfitId;
		std::vector< std::pair<uint32_t, std::string> > m_outfits;
};

#endif
