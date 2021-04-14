#ifndef __TOOLSH__
#define __TOOLSH__

#include <iostream>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "const.h"
#include "structures.h"

class cstring : public std::string {
	public:
		cstring(char *s) : std::string(s) {};
		cstring(std::string s) : std::string(s) {};
		
		cstring lowercase();
		cstring uppercase();
		
		bool equals(cstring& rhs);
		bool operator==(const cstring& rhs);
};

class DirectoryReader {
	private:
		WIN32_FIND_DATAA found; 
		HANDLE hfind;
		char folderstar[256];       
		int chk;

	public:
		DirectoryReader(std::string folder);
		int getNextFile(char* fname);
};

bool isMouseMoveEvent(MouseEvent_t event, bool isButtonDown);
bool isButtonClickEvent(MouseEvent_t event, bool isButtonDown);
bool isButtonReleaseEvent(MouseEvent_t event, bool isButtonDown);

void DEBUG_OUTPUT(std::string filename, std::string output);

bool iequalsn(std::string a, std::string b);
bool iequals(std::string a, std::string b);
std::string ipbintostr(uint32_t ip);
bool IsDots(const TCHAR* str);
bool DeleteDirectory(const TCHAR* sPath);
std::string MilisecondsToStr(uint64_t ms);
uint64_t StrToMiliseconds(std::string str);

int getTotalMetric(int font, std::string message);
std::vector<std::string> wrapTextLines(int m_font, std::string message, int blockWidth);

bool isPointInRectangle(int px, int py, int rx, int ry, int width, int height);
Color_t getHealthPercentColor(int percent);
Color_t getPercentColor(int percent);
void getPingInfo(int ping,std::string& status, uint8_t& red, uint8_t& green, uint8_t &blue);
std::vector<std::string> explodeString(const std::string& inString, const std::string& separator);

bool isHttpResponseCompleted(std::string response);
bool isHttpResponseChunked(std::string response);
bool isHttpResponseOk(std::string response);
std::string getHttpContent(std::string response);
std::string decodeHttpChunkedContent(std::string response);
std::string getHttpContentText(std::string response);

bool isFile(std::string p);

void trim(std::string& source, const std::string& t = " \r\n\t");
std::string trim_s(std::string s);

std::string getClipboardText();
void setClipboardText(const std::string &s);

void ShowMessage(std::string message);
void ShowWarning(std::string message);

#endif
