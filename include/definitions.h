#ifndef __DEFINITIONSH__
#define __DEFINITIONSH__

#ifdef _WIN32_WINNT
	#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0501
#define WINVER 0x0501
#define _WIN32_IE 0x0600

// Undefinied keys
#define VK_A 0x41
#define VK_C 0x43
#define VK_V 0x56
#define VK_X 0x58

#define EAX "eax"
#define EBX "ebx"
#define ECX "ecx"
#define EDX "edx"
#define ESI "esi"
#define EDI "edi"

#define MOV(X, Y) 					\
	asm("mov %" Y ", %" X);
	
#define PUSH(X) 					\
	asm("push %" X);
  
#define POP(X) 					  \
	asm("pop %" X);
	
#define CALL(X, Y)					\
	asm("call *" Y "(%" X ")");

#define SAVE_STACK 					\
	asm(".intel_syntax noprefix"); 	\
	asm("mov _g_lastEsp, esp"); 	\
	asm("mov _g_lastEbp, ebp"); 	\
	asm(".att_syntax noprefix")
	
#define LOAD_STACK 					\
	asm(".intel_syntax noprefix"); 	\
	asm("mov esp, _g_lastEsp"); 	\
	asm("mov ebp, _g_lastEbp"); 	\
	asm(".att_syntax noprefix")
	
#define SAVE_REGISTERS 				\
	asm(".intel_syntax noprefix"); 	\
	asm("mov _g_lastEax, eax"); 	\
	asm("mov _g_lastEbx, ebx"); 	\
	asm("mov _g_lastEcx, ecx"); 	\
	asm("mov _g_lastEdx, edx"); 	\
	asm("mov _g_lastEsi, esi"); 	\
	asm("mov _g_lastEdi, edi"); 	\
	asm(".att_syntax noprefix")
	
#define LOAD_REGISTERS 				\
	asm(".intel_syntax noprefix");	\
	asm("mov eax, _g_lastEax"); 	\
	asm("mov ebx, _g_lastEbx"); 	\
	asm("mov ecx, _g_lastEcx"); 	\
	asm("mov edx, _g_lastEdx"); 	\
	asm("mov esi, _g_lastEsi"); 	\
	asm("mov edi, _g_lastEdi"); 	\
	asm(".att_syntax noprefix")
		
#define SAVE_EAX					\
	asm(".intel_syntax noprefix");	\
	asm("mov _g_lastEax, eax"); 	\
	asm(".att_syntax noprefix")
	
#define SAVE_EBX					\
	asm(".intel_syntax noprefix");	\
	asm("mov _g_lastEbx, ebx"); 	\
	asm(".att_syntax noprefix")
	
#define SAVE_ECX					\
	asm(".intel_syntax noprefix");	\
	asm("mov _g_lastEcx, ecx"); 	\
	asm(".att_syntax noprefix")
	
#define SAVE_EDX					\
	asm(".intel_syntax noprefix");	\
	asm("mov _g_lastEdx, edx"); 	\
	asm(".att_syntax noprefix")
	
#define SAVE_ESI					\
	asm(".intel_syntax noprefix");	\
	asm("mov _g_lastEsi, esi"); 	\
	asm(".att_syntax noprefix")
	
#define SAVE_EDI					\
	asm(".intel_syntax noprefix");	\
	asm("mov _g_lastEdi, edi"); 	\
	asm(".att_syntax noprefix")
	
#define LOAD_EAX					\
	asm(".intel_syntax noprefix");	\
	asm("mov eax, _g_lastEax"); 	\
	asm(".att_syntax noprefix")
	
#define LOAD_EBX					\
	asm(".intel_syntax noprefix");	\
	asm("mov ebx, _g_lastEbx"); 	\
	asm(".att_syntax noprefix")
	
#define LOAD_ECX					\
	asm(".intel_syntax noprefix");	\
	asm("mov ecx, _g_lastEcx"); 	\
	asm(".att_syntax noprefix")
	
#define LOAD_EDX					\
	asm(".intel_syntax noprefix");	\
	asm("mov edx, _g_lastEdx"); 	\
	asm(".att_syntax noprefix")
	
#define LOAD_ESI					\
	asm(".intel_syntax noprefix");	\
	asm("mov esi, _g_lastEsi"); 	\
	asm(".att_syntax noprefix")
	
#define LOAD_EDI					\
	asm(".intel_syntax noprefix");	\
	asm("mov edi, _g_lastEdi"); 	\
	asm(".att_syntax noprefix")

#endif
