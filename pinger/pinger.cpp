#include "pinger.h"
#include "const.h"
#include "thread.h"

Pinger::Pinger(std::string host){
	/* Set default values */
	m_value = 0;
	
	if(!loadIphlpapiLibrary()){
		if(!loadIcmpLibrary()){
			m_lastError = std::string("Pinger was unable to load either Icmp.dll or Iphlpapi.dll");
			m_available = false;
			return;
		}
	}
	
	if((m_phe = gethostbyname(host.c_str())) == NULL){
		m_lastError = std::string("Could not find IP address for ") + host;
		m_available = false;
		return;
	}

	m_hIP = m_pIcmpCreateFile();
	
	if(m_hIP == INVALID_HANDLE_VALUE){
		m_lastError = std::string("Unable to open ping service.");
		m_available = false;
		return;
	}

	// Build ping packet
	m_acPingBuffer = new char[64];
	memset(m_acPingBuffer, '\xAA', 64);
	
	m_pIpe = (PIP_ECHO_REPLY)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(IP_ECHO_REPLY) + sizeof(m_acPingBuffer));
	if(m_pIpe == NULL){
		m_lastError = std::string("Failed to allocate global ping packet buffer.");
		m_available = false;
		return;
	}
	
	m_pIpe->Data = m_acPingBuffer;
	m_pIpe->DataSize = sizeof(m_acPingBuffer);
	
	m_available = true;
	m_running = false;
	m_shutdown = false;
	
	Thread thread(Pinger::ping, this);
	if(!thread.isRunning()){
		m_lastError = std::string("Failed to create thread for pinger.");
		m_available = false;
	}
}

bool Pinger::loadIcmpLibrary(){
	m_hIcmp = LoadLibrary("Icmp.dll");
	
	if(m_hIcmp == NULL){
		return false;
	}
	
	m_pIcmpCreateFile = (pfnHV)GetProcAddress(m_hIcmp, "IcmpCreateFile");
	m_pIcmpCloseHandle = (pfnBH)GetProcAddress(m_hIcmp, "IcmpCloseHandle");
	m_pIcmpSendEcho = (pfnDHDPWPipPDD)GetProcAddress(m_hIcmp, "IcmpSendEcho");
	
	if((m_pIcmpCreateFile == 0) || (m_pIcmpCloseHandle == 0) || (m_pIcmpSendEcho == 0)){
		FreeLibrary(m_hIcmp);
		m_hIcmp = NULL;
		return false;
	}
	
	return true;
}

bool Pinger::loadIphlpapiLibrary(){
	m_hIcmp = LoadLibrary("Iphlpapi.dll");
	
	if(m_hIcmp == NULL){
		return false;
	}
	
	m_pIcmpCreateFile = (pfnHV)GetProcAddress(m_hIcmp, "IcmpCreateFile");
	m_pIcmpCloseHandle = (pfnBH)GetProcAddress(m_hIcmp, "IcmpCloseHandle");
	m_pIcmpSendEcho = (pfnDHDPWPipPDD)GetProcAddress(m_hIcmp, "IcmpSendEcho");
	
	if((m_pIcmpCreateFile == 0) || (m_pIcmpCloseHandle == 0) || (m_pIcmpSendEcho == 0)){
		FreeLibrary(m_hIcmp);
		m_hIcmp = NULL;
		return false;
	}
	
	return true;
}

Pinger::~Pinger(){
	{
		ScopedLock lock(m_statusLock);
		m_shutdown = true;
	}
	
	ScopedLock lock(m_threadLock);
	
	delete[] m_acPingBuffer;
	
	GlobalFree(m_pIpe);
	if(m_hIcmp){
		FreeLibrary(m_hIcmp);
	}
}

std::string Pinger::getLastError(){
  ScopedLock lock(m_valueLock);
  return m_lastError;
}

void Pinger::start(){
	ScopedLock lock(m_statusLock);
	m_running = true;
}

void Pinger::stop(){
	ScopedLock lock(m_statusLock);
	m_running = false;
}

bool Pinger::running(){
	ScopedLock lock(m_statusLock);
	return m_running;
}

bool Pinger::available(){
	return m_available;
}

int Pinger::getPing(){
	ScopedLock lock(m_valueLock);
	return m_value;
}

void Pinger::setHost(std::string host){
	ScopedLock lock(m_hostLock);
	
	if((m_phe = gethostbyname(host.c_str())) == NULL){
		m_lastError = std::string("Could not find IP address for ") + host;
	}
}

DWORD WINAPI Pinger::ping(LPVOID lpParam){
	Pinger* pinger = reinterpret_cast<Pinger*>(lpParam);
	ScopedLock lock(pinger->m_threadLock);
	
	while(true){
		bool m_shutdown;
		bool m_running;
		
		{
			ScopedLock lock(pinger->m_statusLock);
			m_shutdown = pinger->m_shutdown;
			m_running = pinger->m_running;
		}
			
		if(pinger->m_shutdown){
			break;
		}
			
		if(!pinger->m_running){
			Sleep(PING_WAIT);
			continue;
		}
		
		{
			ScopedLock lock(pinger->m_hostLock);
			if(pinger->m_phe == NULL){
				break;
			}
		}
		
		DWORD dwStatus = 0;	
		{
			ScopedLock lock(pinger->m_hostLock);
			dwStatus = pinger->m_pIcmpSendEcho(pinger->m_hIP, *((DWORD*)pinger->m_phe->h_addr_list[0]), pinger->m_acPingBuffer, sizeof(pinger->m_acPingBuffer), NULL, pinger->m_pIpe, sizeof(IP_ECHO_REPLY) + sizeof(pinger->m_acPingBuffer), 5000);
		}
		
		if(dwStatus != 0){
			int RTT = (int)pinger->m_pIpe->RoundTripTime;
			int TTL = (int)pinger->m_pIpe->Options.Ttl;
			
			ScopedLock lock(pinger->m_valueLock);
			pinger->m_value = RTT;
		}
		else {
      ScopedLock lock(pinger->m_valueLock);
			pinger->m_lastError = std::string("Error obtaining info from ping packet.");
		}
		
		Sleep(PING_WAIT);
	}
}
