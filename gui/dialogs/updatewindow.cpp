#include "gui/dialogs/updatewindow.h"
#include "gui/elements/buttons.h"
#include "network/networkmessage.h"
#include "crypt/crc.h"
#include "crypt/base.h"
#include "network/httpget.h"
#include "threads/thread.h"
#include "tools.h"
#include "main.h"

#include <windows.h>
#include <winsock.h>
#include <windowsx.h>
#include <stdio.h>
#include <sstream>

void Exit(Button* button, void* lParam){
	ExitProcess(0);
}

GUIUpdateWindow::GUIUpdateWindow() : GUIWindow("Tibia Patch Updater", 0, 0, 300, 160){
	/* Create basic controls for this window */
	addLabel(CONTROL_1_LOCATION, m_width >> 1, 20 + 14 * 0);	
	addLabel(CONTROL_2_LOCATION, m_width >> 1, 20 + 14 * 1);	
	addLabel(CONTROL_3_LOCATION, m_width >> 1, 20 + 14 * 3);
	addLabel(CONTROL_4_LOCATION, m_width >> 1, 20 + 14 * 5);
	addLabel(CONTROL_5_LOCATION, m_width >> 1, 20 + 14 * 7);
	addProgressBar(CONTROL_6_LOCATION, (m_width >> 1) - 50, 104, 100, 8, 0, 100, 255, 55, 55);
	
	getControl<Label>(CONTROL_1_LOCATION)->setText(Label_t(FONT_BIG, 175, 175, 175, 1), "");
	getControl<Label>(CONTROL_2_LOCATION)->setText(Label_t(FONT_BIG, 175, 175, 175, 1), "Downloading new features...");
	getControl<Label>(CONTROL_3_LOCATION)->setText(Label_t(FONT_BIG, 145, 145, 145, 1), "Status: Initializing...");
	getControl<Label>(CONTROL_4_LOCATION)->setText(Label_t(FONT_NORMAl_1, 145, 145, 145, 1), "File: ...");
	getControl<Label>(CONTROL_5_LOCATION)->setText(Label_t(FONT_NORMAl_1, 145, 145, 145, 1), "0 kB received out of 0 kB (0%)");
	
	/* Hide file name for now */
	getControl<Label>(CONTROL_4_LOCATION)->hide();
	
	/* Hide received bytes */
	getControl<Label>(CONTROL_5_LOCATION)->hide();
	
	/* Progress bar will be only visible if there is something to download */
	getControl<ProgressBar>(CONTROL_6_LOCATION)->hide();
	
	/* Only one button to restart the client, nothing else */
	pushButton("Restart", Exit, NULL, getButtonNextX(), getButtonNextY());
	
	/* Set the current state */
	m_threadStatus = DOWNLOAD_UPDATING;
	
	/* Set default values of the download status */
	m_threadFilePercent = 0;
	m_threadFileBytes = 0;
	m_threadFileReceived = 0;
	
	/* Thread for download itself */
	Thread thread(GUIUpdateWindow::worker, this);
	if(!thread.isRunning()){
    /* Failure - restart button is enabled */
		getControl<Label>(CONTROL_3_LOCATION)->setText("Status: Failed to create thread for connection!");
	} else {
    /* Thread lock - it might be possible that the thread started before execution of this code */
    ScopedLock lock(m_threadLock);
  
    /* Disable button */
		getControl<Buttons>(GUIParent::BUTTONS_1_LOCATION)->getButton(1)->disable();
	}
}

GUIUpdateWindow::~GUIUpdateWindow(){
	ScopedLock lock(m_destructLock);
}

void GUIUpdateWindow::tick(uint32_t ticks){
	/* Dispatch tick among children */
	GUIWindow::tick(ticks);
	
	/* Main lock for thread variables */
	ScopedLock lock(m_threadLock);
	
	switch(m_threadStatus){
		case DOWNLOAD_FINISHED: 
		case DOWNLOAD_ERROR: {
        	if(m_threadInformation.empty()){
				m_threadInformation = std::string("Status: unknown error");
			}
			
			/* Restart button */
			//getControl<Buttons>(GUIParent::BUTTONS_1_LOCATION)->getButton(1)->enable();
			//getControl<Buttons>(GUIParent::BUTTONS_1_LOCATION)->getButton(1)->disable();
			break;
		}
		
		case DOWNLOAD_UPTODATE: {
			GUI::nullDialog(NULL, this);
			break;
		}
		
		default: break;
	}
	
	if(m_threadFileBytes){
		if(getControl<Label>(CONTROL_5_LOCATION)->hidden()){
			getControl<Label>(CONTROL_5_LOCATION)->show();
		}
		
		std::stringstream status;
		status << m_threadFileReceived / 1024 << " kB received out of " << m_threadFileBytes / 1024 << " kB (" << m_threadFilePercent << "%)";
		
		getControl<Label>(CONTROL_5_LOCATION)->setText(status.str());
	}
	
	if(!m_threadInformation.empty()){
		getControl<Label>(CONTROL_3_LOCATION)->setText(m_threadInformation);
	}
	
	if(!m_threadFile.empty()){
		if(getControl<ProgressBar>(CONTROL_6_LOCATION)->hidden()){
			getControl<ProgressBar>(CONTROL_6_LOCATION)->show();
		}
	
		getControl<ProgressBar>(CONTROL_6_LOCATION)->pos(m_threadFilePercent);
	
		if(getControl<Label>(CONTROL_4_LOCATION)->hidden()){
			getControl<Label>(CONTROL_4_LOCATION)->show();
		}
		
		std::string file = std::string("File: ") + m_threadFile;
		getControl<Label>(CONTROL_4_LOCATION)->setText(file);
	}
}

void GUIUpdateWindow::setInformation(std::string threadInformation){
  ScopedLock lock(m_threadLock);
  
  m_threadInformation = std::string("Status: " ) + threadInformation;
}

void GUIUpdateWindow::setStatus(status_t status){
  ScopedLock lock(m_threadLock);
  
  m_threadStatus = status;
}

GUIUpdateWindow::status_t GUIUpdateWindow::getStatus(){
  ScopedLock lock(m_threadLock);
  
  return m_threadStatus;
}

void GUIUpdateWindow::downloader(unsigned long total, unsigned long received){
	GUIUpdateWindow* dialog = g_dll.m_updateDialog;
	
	/* Lock the class */
	ScopedLock lock(dialog->m_threadLock);
	
	dialog->m_threadFilePercent = received * 100 / total;
	dialog->m_threadFileBytes = total;
	dialog->m_threadFileReceived = received;
}

DWORD WINAPI GUIUpdateWindow::worker(LPVOID lpParam){
	GUIUpdateWindow* update = reinterpret_cast<GUIUpdateWindow*>(lpParam);
	
	/* Destructor lock, it will prevent from destructing the class when the thread is still running */
	ScopedLock mainLock(update->m_destructLock);
  
	/* Response variables */
	char response[NETWORKMESSAGE_MAXSIZE];
	uint32_t responseLength = 0;
  
  /* Connecton status */
  TcpConnection::status_t status = TcpConnection::CONNECTION_DISCONNECTED;
  std::string error;
  
  /* Available hosts */
  std::vector<std::string> hosts;
  hosts.push_back("127.0.0.1"); // that's real update ip
  
  for(int attempt = 0; attempt < hosts.size() * 4; attempt++){
    std::string host = hosts[attempt % hosts.size()];
    
    /* Host information */
    update->setInformation(std::string("Connecting to ") + host);
    
    /* Create TCP connection */
    TcpConnection* connection = new TcpConnection(TcpConnection::CONNECTION_TCP);
    connection->tryconnect(host, 80);
    
    /* Create request */
    std::stringstream stream;
    std::string request;
    
    stream << "GET /update/versions.php HTTP/1.0\r\nHost: " << host << "\r\n\r\n";
    request = stream.str();
    
    /* Push request to connection */
    connection->push((char*)request.c_str(), request.length());
    
    /* Lets clear the string so it will be empty */
    memset(response, 0, NETWORKMESSAGE_MAXSIZE);
    
    /* Length of the response */
    responseLength = 0;
    
    while(!isHttpResponseCompleted(response)){
      bool continueNext = true;
      
      /* Fetch connection status */
      status = connection->getStatus();
      error = connection->getError();
      
      switch(status){
        case TcpConnection::CONNECTION_DISCONNECTED:
        case TcpConnection::CONNECTION_ERROR: {
          continueNext = false;
          break;
        }
          
        case TcpConnection::CONNECTION_CONNECTED: {	
          responseLength = responseLength + connection->pop(&response[responseLength]);
          break;
        }
      }
      
      if(!continueNext){
        break;
      }
    }
    
    /* It might take a while to shutdown connection thread */
    delete connection;
    
    /* Perhaps everything went fine? */
    if(isHttpResponseOk(response)){
      break;
    }
    
    /* If the response is fine, we won't get here */
    status = TcpConnection::CONNECTION_ERROR;
    error = Crypt::Base::decode("X=529:~*/~;/009;*"); // Failed to connect
  }
  
  /* Check connection status */
  if(status == TcpConnection::CONNECTION_ERROR){
    update->setInformation(error);
    update->setStatus(DOWNLOAD_ERROR);
  }
  
  /* Response content */
  std::string responseContent = getHttpContentText(response);
  
  /* Is the body of the response empty? */
  if(responseContent.empty()){
    update->setStatus(DOWNLOAD_ERROR);
  }
	
  /* There is no need to go through the response, since apparently there was some error */
  if(update->getStatus() == DOWNLOAD_ERROR){
    return 0;
  }
	
	/* Everything seems to be fine */
	std::vector<std::string> files = explodeString(responseContent, "\n");
  
  update->setInformation("files received, validating...");
	
	std::vector<std::string> list;
	for(std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it){
		if(it->empty()){
			continue;
		}
		
		std::stringstream stream(*it);
		
		std::string filename;
		uint32_t checksum;
		
		/* Pop from the stream */
		stream >> filename;
		stream >> checksum;
		
		std::string path;
		if(!iequals(filename, MAIN_EXECUTABLE)){
			path = std::string(GAME_DIR) + filename;
		} else {
			path = g_dll.m_moduleName;
		}
		
		if(crc32(path) != checksum){
			list.push_back(filename);
		}
	}
	
	/* Remove old directory here */
	if(!DeleteDirectory(OUTDATE_DIR)){
		DWORD dwError = GetLastError();
		if(dwError != ERROR_PATH_NOT_FOUND){
			update->setInformation(std::string("unable to delete ") + std::string(OUTDATE_DIR));
			update->setStatus(DOWNLOAD_ERROR);
			return 0;
		}
	}

	if(list.empty()){  
    update->setInformation("the client is up to date.");
    update->setStatus(DOWNLOAD_UPTODATE);
		return 0;
	}
  
  update->setInformation("updating files...");
	
	if(!CreateDirectory(OUTDATE_DIR, NULL)){
		DWORD dwError = GetLastError();
		if(dwError != ERROR_ALREADY_EXISTS){
      update->setInformation(std::string("unable to create ") + std::string(OUTDATE_DIR));
      update->setStatus(DOWNLOAD_ERROR);
			return 0;
		}
	}
	
	while(!list.empty()){
		std::string outdate = std::string(OUTDATE_DIR) + std::string("/") + *list.begin();
		
		if(isFile(outdate)){
			if(!DeleteFile(outdate.c_str())){
        update->setInformation(std::string("unable to delete ") + outdate);
        update->setStatus(DOWNLOAD_ERROR);
				return 0;
			}
		}
		
		std::string path;
		if(!iequals(*list.begin(), MAIN_EXECUTABLE)){
			path = std::string(GAME_DIR) + *list.begin();
		} else {
			path = g_dll.m_moduleName;
		}
		
		if(!MoveFile(path.c_str(), outdate.c_str())){
			DWORD dwError = GetLastError();
			if(dwError != ERROR_FILE_NOT_FOUND){
        update->setInformation(std::string("unable to move ") + list.begin()->c_str());
        update->setStatus(DOWNLOAD_ERROR);
				return 0;
			}
		}
		
		/* Lets set variables regarding current file */
		{
			ScopedLock lock(update->m_threadLock);
			
			update->m_threadFile = *list.begin();
			update->m_threadFilePercent = 0;
			update->m_threadFileBytes = 0;
			update->m_threadFileReceived = 0;
		}
		
		/* Now, lets try to download a new file */
		try {
			httpGet::download((char*)std::string(std::string(UPDATER_URL) + *list.begin()).c_str(), (char*)std::string(path).c_str(), false, &GUIUpdateWindow::downloader);
		} catch(DLExc error){
      update->setInformation(std::string(error.geterr()));
      update->setStatus(DOWNLOAD_ERROR);
			return 0;
		}
		
		list.erase(list.begin());
	}
	
  update->setInformation("restart your client.");
  update->setStatus(DOWNLOAD_FINISHED);
}
