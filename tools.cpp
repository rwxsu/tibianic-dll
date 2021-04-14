#include "tools.h"
#include "main.h"
#include "functions.h"

#include <io.h>
#include <sys/stat.h>

bool isMouseMoveEvent(MouseEvent_t event, bool isButtonDown){
	return event == EVENT_MOVEMENT && isButtonDown;
}

bool isButtonClickEvent(MouseEvent_t event, bool isButtonDown){
	return event == EVENT_BUTTON && isButtonDown;
}

bool isButtonReleaseEvent(MouseEvent_t event, bool isButtonDown){
	return event == EVENT_BUTTON && !isButtonDown;
}

void DEBUG_OUTPUT(std::string filename, std::string output){
	FILE* f = fopen(filename.c_str(), "w");
	if(f){
		fputs(output.c_str(), f);
		fclose(f);
	}
}

bool iequalsn(std::string a, std::string b){
    if (a.length() < b.length()){
        return false;
	}
	
    for (unsigned int i = 0; i < b.length(); ++i){
        if (tolower(a[i]) != tolower(b[i])){
            return false;
		}
	}
	
    return true;
}

bool iequals(std::string a, std::string b){
    unsigned int sz = a.size();
    if (b.size() != sz){
        return false;
	}
	
    for (unsigned int i = 0; i < sz; ++i){
        if (tolower(a[i]) != tolower(b[i])){
            return false;
		}
	}
	
    return true;
}

std::string ipbintostr(uint32_t ip){
	char buf[64];
	int a = (uint8_t)(ip >> 0);
	int b = (uint8_t)(ip >> 8);
	int c = (uint8_t)(ip >> 16);
	int d = (uint8_t)(ip >> 24);
	
	sprintf(buf, "%d.%d.%d.%d", a, b, c, d);
	
	return std::string(buf);
}

bool IsDots(const TCHAR* str){
    if(strcmp(str,".") && strcmp(str,"..")) return false;
    return true;
}

bool DeleteDirectory(const TCHAR* sPath){
    HANDLE hFind;  // file handle
    WIN32_FIND_DATA FindFileData;

    TCHAR DirPath[MAX_PATH];
    TCHAR FileName[MAX_PATH];

    strcpy(DirPath,sPath);
    strcat(DirPath,"\\*");    // searching all files
    strcpy(FileName,sPath);
    strcat(FileName,"\\");

    hFind = FindFirstFile(DirPath,&FindFileData); // find the first file
    if(hFind == INVALID_HANDLE_VALUE) return FALSE;
    strcpy(DirPath,FileName);
        
    bool bSearch = true;
    while(bSearch) { // until we finds an entry
        if(FindNextFile(hFind,&FindFileData)) {
            if(IsDots(FindFileData.cFileName)) continue;
            strcat(FileName,FindFileData.cFileName);
            if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

                // we have found a directory, recurse
                if(!DeleteDirectory(FileName)) { 
                    FindClose(hFind); 
                    return FALSE; // directory couldn't be deleted
                }
                RemoveDirectory(FileName); // remove the empty directory
                strcpy(FileName,DirPath);
            }
            else {
                if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
                    _chmod(FileName, _S_IWRITE); // change read-only file mode
                if(!DeleteFile(FileName)) {  // delete the file
                    FindClose(hFind); 
                    return FALSE; 
                }                 
                strcpy(FileName,DirPath);
            }
        }
        else {
            if(GetLastError() == ERROR_NO_MORE_FILES) // no more files there
            bSearch = false;
            else {
                // some error occured, close the handle and return FALSE
                FindClose(hFind); 
                return FALSE;
            }

        }

    }
    FindClose(hFind);  // closing file handle
 
    return RemoveDirectory(sPath); // remove the empty directory

}

std::string MilisecondsToStr(uint64_t ms){
	int seconds = (ms / 1000) % 60 ;
	int minutes = (ms / (1000*60)) % 60;
	int hours   = (ms / (1000*60*60)) % 24;
	
	std::stringstream ss;
	ss.precision(2);
	
	if(hours < 10){
		ss << "0";
	}	
	ss << hours << ":";
	
	if(minutes < 10){
		ss << "0";
	}	
	ss << minutes << ":";
	
	if(seconds < 10){
		ss << "0";
	}	
	ss << seconds;
	
	return ss.str();
}

uint64_t StrToMiliseconds(std::string str){
	int seconds, minutes, hours;
	sscanf(str.c_str(), "ff %d:%d:%d", &hours, &minutes, &seconds);
	
	uint64_t ret = 0;
	ret += seconds * 1000;
	ret += minutes * 60 * 1000;
	ret += hours * 60 * 60 * 1000;
	
	return ret;
}

int getTotalMetric(int font, std::string message){
	return Tibia::TextMetric(font, (char*)message.c_str(), message.length());
}

std::vector<std::string> wrapTextLines(int m_font, std::string message, int blockWidth){
	std::vector<std::string> m_lines;

	std::string m_lastword;
	std::string m_prevword;

	while(!message.empty()){
		std::string m_line;

		while(getTotalMetric(m_font, m_line) <= blockWidth && !message.empty()){
			std::string m_word;
			bool m_lineFinish = false;

			if(m_lastword.empty()){
				for(int i = 0; i < message.length(); i++){
					char c = message[i];
					if(c == ' '){
						message = message.substr(i + 1);
						break;
					} else if(c == '\n'){
						message = message.substr(i + 1);
						m_lineFinish = true;
						break;
					}

					m_word += c;
					if(i == message.length() - 1){
						message = std::string();
					}
				}
			} else {
				m_word = m_lastword;
				m_lastword = std::string();
			}
			
			if(getTotalMetric(m_font, m_line) + getTotalMetric(m_font, m_word) > blockWidth){
				m_lastword = m_word;
				break;
			}

			if(m_lineFinish){
				//m_prevword = m_word;
				if(!m_word.empty()){
					if(!m_line.empty()){
						m_line = m_line + std::string(" ");
					}
					m_line = m_line + m_word;
				}
				
				break;
			}

			if(!m_prevword.empty()){
				message = message + std::string(" ") + m_word;
				m_word = m_prevword;
				m_prevword = std::string();
			}

			if(!m_word.empty()){
				if(!m_line.empty()){
					m_line = m_line + std::string(" ");
				}

				m_line = m_line + m_word;

			}
		}

		m_lines.push_back(m_line);
	}

	if(!m_lastword.empty()){
		m_lines.push_back(m_lastword);
	}

	return m_lines;
}

cstring cstring::lowercase(){
	cstring v = *this;
	std::transform(v.begin(), v.end(), v.begin(), tolower);
	
	return v;
}

cstring cstring::uppercase(){
	cstring v = *this;
	std::transform(v.begin(), v.end(), v.begin(), toupper);
	
	return v;
}

bool cstring::equals(cstring& rhs){
	if(compare(rhs) == 0){
		return true;
	}
	
	return false;
}

bool cstring::operator==(const cstring& rhs){
	return compare(rhs);
}

DirectoryReader::DirectoryReader(std::string folder){      
	sprintf(folderstar, "%s\\*.*", folder.c_str());
	hfind = FindFirstFileA(folderstar, &found);

	FindNextFileA(hfind, &found);        
}

int DirectoryReader::getNextFile(char* fname){
	if(chk = FindNextFileA(hfind, &found)){
		strcpy(fname, found.cFileName);    
	}
	
	return chk;
}

void ShowMessage(std::string message){
	MessageBox(NULL, message.c_str(), MESSAGE_TITLE, MB_ICONINFORMATION | MB_OK);
}

void ShowWarning(std::string message){
	MessageBox(NULL, message.c_str(), MESSAGE_TITLE, MB_ICONWARNING | MB_OK);
}

bool isPointInRectangle(int px, int py, int rx, int ry, int width, int height){
	if(px < rx || py < ry || px > rx + width || py > ry + height){
		return false;
	}
	
	return true;
}

Color_t getHealthPercentColor(int percent){
	if(percent >= 95){
		return Color_t(0, 188, 0);
	} else if(percent >= 60){
		return Color_t(96, 192, 96);
	} else if(percent >= 30){
		return Color_t(192, 192, 0);
	} else if(percent >= 10){
		return Color_t(192, 48, 48);
	} else if(percent >= 5){
		return Color_t(192, 0, 0);
	}
	
	return Color_t(95, 0, 0);
}

Color_t getPercentColor(int percent){
	if(percent >= 100){
		return Color_t(80, 255, 0);
	} else if(percent >= 90){
		return Color_t(100, 240, 0);
	} else if(percent >= 80){
		return Color_t(120, 220, 0);
	} else if(percent >= 70){
		return Color_t(140, 200, 0);
	} else if(percent >= 60){
		return Color_t(160, 180, 0);
	} else if(percent >= 50){
		return Color_t(180, 160, 0);
	} else if(percent >= 40){
		return Color_t(200, 140, 0);
	} else if(percent >= 30){
		return Color_t(220, 120, 0);
	}  else if(percent >= 20){
		return Color_t(240, 100, 0);
	} else if(percent >= 10){
		return Color_t(255, 100, 0);
	}
	
	return Color_t(255, 80, 0);
}

void getPingInfo(int ping,std::string& status, uint8_t& red, uint8_t& green, uint8_t &blue){
	red = green = blue = 0;
	
	if(ping <= 80){
		red = 100;
		green = 255;
		status = std::string("Good");
	} else if(ping <= 160){
		red = 255;
		green = 255;
		status = std::string("Average");
	} else {
		red = 255;
		green = 52;
		blue = 52;
		status = std::string("Bad");
	}
}

/*
void getPingColor(int ping, uint8_t& red, uint8_t& green, uint8_t &blue){
	if(ping >= 250){
		red = 255;
		green = 0;
		blue = 0;
	} else if(ping >= 200){
		red = 255;
		green = 40;
		blue = 0;
	} else if(ping >= 150){
		red = 255;
		green = 80;
		blue = 0;
	} else if(ping >= 100){
		red = 255;
		green = 120;
		blue = 0;
	} else if(ping >= 90){
		red = 255;
		green = 160;
		blue = 0;
	} else if(ping >= 80){
		red = 255;
		green = 200;
		blue = 0;
	} else if(ping >= 70){
		red = 220;
		green = 240;
		blue = 0;
	} else if(ping >= 60){
		red = 255;
		green = 255;
		blue = 0;
	} else if(ping >= 50){
		red = 200;
		green = 255;
		blue = 0;
	} else if(ping >= 40){
		red = 150;
		green = 255;
		blue = 0;
	} else {
		red = 100;
		green = 255;
		blue = 0;
	}
}
*/

std::vector<std::string> explodeString(const std::string& inString, const std::string& separator){
	std::vector<std::string> returnVector;
	std::string::size_type start = 0;
	std::string::size_type end = 0;

	while((end=inString.find (separator, start)) != std::string::npos){
		returnVector.push_back (inString.substr (start, end-start));
		start = end+separator.size();
	}

	returnVector.push_back (inString.substr (start));
	return returnVector;
}

bool isHttpResponseCompleted(std::string response){
	if(response.find("\r\n\r\n") == std::string::npos){
		return false;
	}
	
	// pierwszy if nie powinien sie nigdy spelnic no chyba, ze response nie wymaga contentu
	if(response.find("Content-Length") == std::string::npos){
		if(response.find("Connection: close") == std::string::npos && response.find("Transfer-Encoding") == std::string::npos){
			return false;
		}
		
		size_t transferEncoding = response.find("Transfer-Encoding");
		if(transferEncoding != std::string::npos){
			std::string tmp = response.substr(transferEncoding);
			std::string encoding = tmp.substr(0, tmp.find("\n"));
			trim(encoding);

			std::string type = encoding.substr(encoding.find(":") + 1);
			trim(type);

			if(type.compare("chunked") == 0){
				return response.find("0\r\n\r\n") != std::string::npos;
			}
		}
		
		// We need more things in here but for now its false
		return false;
	} else {
		std::string length = response.substr(response.find("Content-Length") + std::string("Content-Length:").length());
		int value = atoi(length.substr(0, length.find("\r\n")).c_str());
		
		std::string content = response.substr(response.find("\r\n\r\n") + 4);
		
		if(content.length() < value){
			return false;
		}
	}
	
	return true;
}

bool isHttpResponseChunked(std::string response){
	size_t transferEncoding = response.find("Transfer-Encoding");
	
	if(transferEncoding != std::string::npos){
		std::string tmp = response.substr(transferEncoding);
		std::string encoding = tmp.substr(0, tmp.find("\n"));
		trim(encoding);

		std::string type = encoding.substr(encoding.find(":") + 1);
		trim(type);

		if(type.compare("chunked") == 0){
			return response.find("0\r\n\r\n") != std::string::npos;
		}
	}
	
	return false;
}

bool isHttpResponseOk(std::string response){
  // HTTP/1.0 200 OK
  std::stringstream httpStream(response);
  
  std::string httpVersion;
  std::string httpString;
  uint32_t httpStatus = 404;
  
  httpStream >> httpVersion >> httpStatus >> httpString;
  
  if(httpVersion.compare("HTTP/1.1") == 0 || httpVersion.compare("HTTP/1.0") == 0){
    if(httpStatus == 200){
      if(httpString.compare("OK") == 0){
        return true;
      }
    }		
  }
  
  return false;
}

std::string getHttpContent(std::string response){
  if(response.find("\r\n\r\n") != std::string::npos){
    return response.substr(response.find("\r\n\r\n") + 4);
  }
  
  return std::string();
}

std::string decodeHttpChunkedContent(std::string response){
	std::vector<std::string> lines = explodeString(response, "\r\n");

	std::string ret;
	while(lines.size()){
		std::string size = *lines.begin();
		lines.erase(lines.begin());
		
		if(size.empty()){
			continue;
		}

		unsigned int chunkLength;
		std::stringstream ss;
		ss << std::hex << size;
		ss >> chunkLength;
		
		if(chunkLength == 0){ // end of chunked encoding
			break;
		}

		while(chunkLength){
			if(lines.empty()){
				break;
			}

			std::string text = *lines.begin();
			lines.erase(lines.begin());
			
			chunkLength = chunkLength - std::min(chunkLength, text.length());		
			ret.append(text);
		}
	}

	return ret;
}

std::string getHttpContentText(std::string response){
	std::string content = getHttpContent(response);
	if(isHttpResponseChunked(response)){
		return decodeHttpChunkedContent(content);
	}
	
	return content;
}


bool isFile(std::string p){
	FILE *f = fopen(p.c_str(), "rb");
	if(!f){
		return false;
	}
	
	fclose(f);
	return true;
}

void trim_right(std::string& source, const std::string& t){
	source.erase(source.find_last_not_of(t)+1);
}

void trim_left(std::string& source, const std::string& t){
	source.erase(0, source.find_first_not_of(t));
}

void trim(std::string& source, const std::string& t){
	trim_left(source, t);
	trim_right(source, t);
}

std::string trim_s(std::string s){
	trim(s);
	return s;
}

std::string getClipboardText(){
	std::string tmp;
	
	HANDLE ClipboardData;

	if(OpenClipboard(0) != 0){
		if((ClipboardData = GetClipboardData(CF_TEXT)) != 0){
			tmp.append((const char*)ClipboardData);
		}

		CloseClipboard();
	}
	
	return tmp;
}

void setClipboardText(const std::string &s){
	if(OpenClipboard(0) != 0){
		EmptyClipboard();
		
		HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
		
		if(!hg){
			CloseClipboard();
			return;
		}
		
		memset(GlobalLock(hg), 0, s.size() + 1);
		memcpy(GlobalLock(hg), s.c_str(), s.size());
		GlobalUnlock(hg);
		SetClipboardData(CF_TEXT, hg);
		CloseClipboard();
		GlobalFree(hg);
	}
}

