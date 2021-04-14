 #ifndef __HTTPGETH__
#define __HTTPGETH__

#include <iostream>
#include <string>
#include <windows.h>
#include <wininet.h>
#include <fstream>

const int MAX_ERRMSG_SIZE = 80;
const int MAX_FILENAME_SIZE = 512;
const int BUF_SIZE = 10240; // 10 KB

class DLExc {
	private:
		char err[MAX_ERRMSG_SIZE];
	public:
		DLExc(const char *exc){
			if(strlen(exc) < MAX_ERRMSG_SIZE)
			strcpy(err, exc);
		}
		
		DLExc(char *exc){
			if(strlen(exc) < MAX_ERRMSG_SIZE)
			strcpy(err, exc);
		}

		const char *geterr(){
			return err;
		}
};

class httpGet {
	private:
		static bool ishttp(char *url);
		static unsigned long openfile(char *fname, bool reload, std::ofstream &fout);
	public:
		static bool download(char *url, char *output, bool reload=false, void (*update)(unsigned long, unsigned long) = NULL);
};


#endif
