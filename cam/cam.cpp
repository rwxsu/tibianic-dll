#include "cam.h"

CAM::CAM(){
	player = new Player();
	recorder = new Recorder();
	
	m_enabled = false;
	m_which = false;
}

CAM::~CAM(){
	delete player;
	delete recorder;
}

void CAM::enablePlayer(){
	if(isRecorderEnabled()){
		disableRecorder();
	}
	
	if(!isPlayerEnabled()){
		m_enabled = true;
		m_which = true;
		// do stuff like create thread for cam server + change ip
	}
}

void CAM::enableRecorder(){
	if(isPlayerEnabled()){
		disablePlayer();
	}
	
	if(!isRecorderEnabled()){
		m_enabled = true;
		m_which = false;
	}
}

void CAM::disablePlayer(){
	if(isPlayerEnabled()){
		m_enabled = false;
		// do stuff like change ip again and shutdown cam server
	}
}

void CAM::disableRecorder(){
	if(isRecorderEnabled()){
		m_enabled = false;
	}
}

bool CAM::isPlayerEnabled(){
	return m_enabled && m_which;
}

bool CAM::isRecorderEnabled(){
	return m_enabled && !m_which;
}
