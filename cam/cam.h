#ifndef __CAMH__
#define __CAMH__

#include <iostream>

#include "recorder.h"
#include "player.h"

class CAM {
	public:
		CAM();
		~CAM();
		
		Recorder* getRecorder(){ return recorder; }
		Player* getPlayer(){ return player; }
		
		void enablePlayer();
		void enableRecorder();
		
		void disablePlayer();
		void disableRecorder();
		
		bool isPlayerEnabled();
		bool isRecorderEnabled();
	private:
		Recorder* recorder;
		Player* player;
		
		bool m_enabled;
		bool m_which; // 0 = recorder, 1 = player
};

#endif
