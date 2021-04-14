#include "httpget.h"

bool httpGet::download(char *url, char *output, bool reload, void (*update)(unsigned long, unsigned long)){
	std::ofstream fout;         // output stream
	unsigned char buf[BUF_SIZE];// input buffer
	unsigned long numrcved;     // number of bytes read
	unsigned long filelen;      // length of the file on disk
	HINTERNET hIurl, hInet;     // internet handles
	unsigned long contentlen;   // length of content
	unsigned long len;          // length of contentlen
	unsigned long total = 0;    // running total of bytes received
	char header[80];            // holds Range header

	try {
		if(!ishttp(url)){
			throw DLExc("URL is not http!");
		}

		filelen = openfile(output, reload, fout);

		if(InternetAttemptConnect(0) != ERROR_SUCCESS){
			throw DLExc("internet connection is not available.");
		}

		hInet = InternetOpen("Tibia Client", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		
		if(hInet == NULL){
			throw DLExc("failed to open internet connetion.");
		}

		sprintf(header, "Range:bytes=%d-", filelen);

		hIurl = InternetOpenUrl(hInet, url, header, strlen(header), INTERNET_FLAG_NO_CACHE_WRITE, 0);
		
		if(hIurl == NULL){
			throw DLExc("failed to open URL.");
		}
		
		len = sizeof contentlen;
		
		if(!HttpQueryInfo(hIurl, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &contentlen, &len, NULL)){
			throw DLExc("failed to get content length.");
		}

		if(filelen != contentlen && contentlen){
			do {
				if(!InternetReadFile(hIurl, &buf, BUF_SIZE, &numrcved)){
					throw DLExc("error occurred during download.");
				}

				fout.write((const char *) buf, numrcved);
				
				if(!fout.good()){
					throw DLExc("error occurred while writing file.");
				}

				total += numrcved;

				if(update && numrcved > 0){
					update(contentlen + filelen, total + filelen);
				}
			} while (numrcved > 0);		
		} else {
			if(update){
				update(filelen, filelen);
			}
		}
	} catch (DLExc) {
		fout.close();
		InternetCloseHandle(hIurl);
		InternetCloseHandle(hInet);

		throw;
	}

	fout.close();
	InternetCloseHandle(hIurl);
	InternetCloseHandle(hInet);

	return true;
}

unsigned long httpGet::openfile(char *fname, bool reload, std::ofstream &fout){
	if(!reload){
		fout.open(fname, std::ios::binary | std::ios::out | std::ios::app | std::ios::ate);
	} else {
		fout.open(fname, std::ios::binary | std::ios::out | std::ios::trunc);
	}

	if(!fout){
		throw DLExc("unable to open output file.");
	}

	return fout.tellp();
}

bool httpGet::ishttp(char *url){
	char str[5] = "";

	strncpy(str, url, 4);

	for(char *p = str; *p; p++){
		*p = tolower(*p);
	}

	return !strcmp("http", str);
}
