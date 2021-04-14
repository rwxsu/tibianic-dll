#ifndef __UPDATEWINDOWH__
#define __UPDATEWINDOWH__

#include "guiwindow.h"
#include "lock.h"

#include <map>

class GUIUpdateWindow : public GUIWindow {
	public:
		enum status_t {
			DOWNLOAD_ERROR,
			DOWNLOAD_UPDATING,
			DOWNLOAD_UPTODATE,
			DOWNLOAD_FINISHED,
		};
		
		/* Constructor and destructor */
		GUIUpdateWindow();
		~GUIUpdateWindow();
		
		/* Override virtual function */
		void tick(uint32_t ticks);
    
		/* Main static function as working thread */
		static DWORD WINAPI worker(LPVOID lpParam);
		
		/* Helper static function to update status of the file */
		static void downloader(unsigned long received, unsigned long total);
  protected:
    /* Set status information */
    void setInformation(std::string threadInformation);
    
    /* Set status of the connection */
    void setStatus(status_t status);
    
    /* Get status of the connection */
    status_t getStatus();
	private:
		/* Thread variables, unsafe to access from main thread */
		std::string m_threadInformation;
		std::string m_threadFile;
		uint32_t m_threadFilePercent;
		uint32_t m_threadFileBytes;
		uint32_t m_threadFileReceived;
		status_t m_threadStatus;
		
		/* Common variables */
		Lock m_threadLock;
		Lock m_destructLock;
};

#endif
