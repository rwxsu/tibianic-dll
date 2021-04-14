#include "newswindow.h"
//#include "buttons.h"
#include "main.h"
#include "tools.h"
#include "networkmessage.h"
#include "thread.h"

#include <windows.h>
#include <winsock.h>
#include <windowsx.h>

void readNews(Button* button, void* param){
	std::string* topic = reinterpret_cast<std::string*>(param);
	
	/* Create link */
	char buffer[1024];
	sprintf(buffer, "http://tibia.com/news.php", topic->c_str());
	
	/* Open website */
	ShellExecute(NULL, "open", buffer, NULL, NULL, SW_SHOWDEFAULT);
}

GUINewsWindow::GUINewsWindow() : GUIWindow("Newest Patch Info:", 0, 0, 550, 200) {
	GUIWindow::pushButton("Close", &GUI::nullDialog, NULL, GUIWindow::getButtonNextX(), GUIWindow::getButtonNextY());
	GUIWindow::pushButton("Read more", readNews, &m_topic, GUIWindow::getButtonNextX(), GUIWindow::getButtonNextY());
	
	/* Create basic controls */
	GUIParent::addLabel(CONTROL_1_LOCATION, m_width >> 1, 22);
	GUIParent::addMemoBox(CONTROL_2_LOCATION, 12, 40, 530, 10);
	
	/* Set default values */
	GUIParent::getControl<Label>(CONTROL_1_LOCATION)->setText(Label_t(FONT_BIG, 155, 155, 155, 1), "Status: awaiting...");
	
	/* There is no connection yet */
	m_connection = NULL;
	
	/* Just waiting for things to happen */
	m_status = FETCH_AWAITING;
}

GUINewsWindow::~GUINewsWindow(){
	if(m_connection){
		delete m_connection;
	}
}

void GUINewsWindow::fetch(){
	// In case if there isn't anything to read
	getControl<Buttons>(GUIParent::BUTTONS_1_LOCATION)->getButton(2)->disable();
	
	// Set default values
	m_status = FETCH_CONNECTING;
	m_topic = std::string();
	
	// Set controls values
	getControl<Label>(CONTROL_1_LOCATION)->setText(Label_t(FONT_BIG, 155, 155, 155, 1), "Status: connecting...");
	getControl<MemoBox>(CONTROL_2_LOCATION)->setText("");
	
	// There might be an old connection so we need to shut it down
	if(m_connection){
		delete m_connection;
	}
	
	// Connection will be created later on
	m_connection = NULL;
	
	// Clear the response
	memset(m_response, 0, NETWORKMESSAGE_MAXSIZE);
	m_responseLength = 0;
}

void GUINewsWindow::tick(uint32_t ticks){
	GUIWindow::tick(ticks);
	
	switch(m_status){
		case FETCH_CONNECTING: {
			if(m_connection == NULL){
				m_connection = new TcpConnection(TcpConnection::CONNECTION_TCP);
				m_connection->tryconnect("tibia.com", 80);
				
				char request[1024];
				sprintf(request, "GET /client_news.php HTTP/1.1\r\nHost: tibia.com\r\n\r\n");
	
				m_connection->push(request, strlen(request));
			}
			
			switch(m_connection->getStatus()){
				case TcpConnection::CONNECTION_DISCONNECTED: {
					m_status = FETCH_FINISHED;				
					break;
				}
				
				case TcpConnection::CONNECTION_ERROR: {
					m_status = FETCH_ERROR;				
					break;
				}
				
				case TcpConnection::CONNECTION_CONNECTED: {	
					if(!isHttpResponseCompleted(m_response)){
						m_responseLength = m_responseLength + m_connection->pop(&m_response[m_responseLength]);
					} else {
						m_status = FETCH_FINISHED;
					}
	
					break;
				}
				
				default: break;
			}
			
			getControl<Label>(CONTROL_1_LOCATION)->setText(std::string("Status: ") + m_connection->getError());
			break;
		}
		
		case FETCH_ERROR: {
			getControl<Label>(CONTROL_1_LOCATION)->setText(std::string("Status: ") + m_connection->getError());
			m_status = FETCH_AWAITING;
			break;
		}
		
		case FETCH_FINISHED: {
			getControl<Label>(CONTROL_1_LOCATION)->setText(std::string("Status: successfull."));
			
			std::string response = getHttpContent(m_response);
			if(!response.empty()){
				m_topic = response.substr(0, response.find("\n"));	
				response = response.substr(response.find("\n") + 1);
				
				// Set text of the news
				getControl<MemoBox>(CONTROL_2_LOCATION)->setText(response);
				
				// Enable read more button
				getControl<Buttons>(GUIParent::BUTTONS_1_LOCATION)->getButton(2)->enable();
			}
			
			// Set idle status
			m_status = FETCH_AWAITING;
			break;
		}
		
		default: break;
	}
}
