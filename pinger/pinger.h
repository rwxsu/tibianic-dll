#ifndef __PINGERH__
#define __PINGERH__

#include <iostream>
#include <windows.h>
#include <winsock.h>
#include <windowsx.h>

#include "lock.h"

typedef struct {
    unsigned char Ttl;                         // Time To Live
    unsigned char Tos;                         // Type Of Service
    unsigned char Flags;                       // IP header flags
    unsigned char OptionsSize;                 // Size in bytes of options data
    unsigned char *OptionsData;                // Pointer to options data
} IP_OPTION_INFORMATION, * PIP_OPTION_INFORMATION;

typedef struct {
    DWORD Address;                             // Replying address
    unsigned long  Status;                     // Reply status
    unsigned long  RoundTripTime;              // RTT in milliseconds
    unsigned short DataSize;                   // Echo data size
    unsigned short Reserved;                   // Reserved for system use
    void *Data;                                // Pointer to the echo data
    IP_OPTION_INFORMATION Options;             // Reply options
} IP_ECHO_REPLY, * PIP_ECHO_REPLY;

typedef HANDLE (WINAPI* pfnHV)(VOID);
typedef BOOL (WINAPI* pfnBH)(HANDLE);
typedef DWORD (WINAPI* pfnDHDPWPipPDD)(HANDLE, DWORD, LPVOID, WORD, PIP_OPTION_INFORMATION, LPVOID, DWORD, DWORD);

class Pinger {
	public:
		Pinger(std::string host);
		~Pinger();
		
		/* Basic actions */
		void start();
		void stop();
		bool running();
		bool available();
		
		/* Get last error */
		std::string getLastError();
		
		/* Set current ping host */
		void setHost(std::string host);
		
		/* Get current ping value */
		int getPing();
		
		/* Static method for threads */
		static DWORD WINAPI ping(LPVOID lpParam);
	protected:
		bool loadIcmpLibrary();
		bool loadIphlpapiLibrary();
	private:
		Lock m_statusLock;
		Lock m_valueLock;
		Lock m_hostLock;
		Lock m_threadLock;
		
		bool m_shutdown;
		bool m_running;
		bool m_available;
		
		std::string m_lastError;
		std::string m_host;
		
		/* Ping packet */
		char* m_acPingBuffer;
		PIP_ECHO_REPLY m_pIpe;
		
		/* Target host variables */
		hostent* m_phe;
		HANDLE m_hIP;
		
		/* Loaded library */
		HINSTANCE m_hIcmp;
		
		/* Loaded library functions */
		pfnHV m_pIcmpCreateFile;
		pfnBH m_pIcmpCloseHandle;
		pfnDHDPWPipPDD m_pIcmpSendEcho;
		
		/* Current ping value */
		int m_value;
};

#endif
