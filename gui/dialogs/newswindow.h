#ifndef __NEWSWINDOWH__
#define __NEWSWINDOWH__

#include <vector>

#include "const.h"
#include "guiwindow.h"
#include "lock.h"
#include "tcpconnection.h"

class GUINewsWindow : public GUIWindow {
	public:
		enum status_t {
			FETCH_ERROR,
			FETCH_CONNECTING,
			FETCH_FINISHED,
			FETCH_AWAITING,
		};
		
		/* Constructor and destructor */
		GUINewsWindow();
		~GUINewsWindow();
		
		/* Method to fetch news */
		void fetch();
		
		/* Override virtual function */
		void tick(uint32_t ticks);
	private:
		/* Id of the topic which was fetched */
		std::string m_topic;
		
		/* Status of the fetching process */
		status_t m_status;
		
		/* Main connection for request */
		TcpConnection* m_connection;
		
		/* Http response related variables */
		char m_response[NETWORKMESSAGE_MAXSIZE];
		uint32_t m_responseLength;
};

#endif
