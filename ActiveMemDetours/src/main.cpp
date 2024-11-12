#include <stdio.h>
#include <windows.h>
#include "detours.h"
#include "syelog.h"
#include "util.h"
#include "true_funcs.h"
#include <DbgHelp.h>
#include <intrin.h>
// #include <vector>

#pragma intrinsic(_ReturnAddress)

volatile BOOL g_isMonitored = TRUE;
// auto g_vector = std::vector<>

VOID* MyHeapAlloc(HANDLE hHeap, DWORD dwFlags,SIZE_T dwBytes);
BOOL MyHeapFree(HANDLE hHeap,DWORD dwFlags,VOID* lpMem);

void WalkStack(CONTEXT* context) {
	DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
	HANDLE currProcess = GetCurrentProcess();
	HANDLE currThread = GetCurrentThread();
	STACKFRAME64 stackFrame;
	stackFrame.AddrPC.Offset = context->Rip;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = context->Rsp;
	stackFrame.AddrStack.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = context->Rbp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;

	while(StackWalk64(machineType, currProcess, currThread, &stackFrame, context, NULL, NULL, NULL, NULL)) {
		Syelog(SYELOG_SEVERITY_CRITICAL, " -- Ret: %lx\n", stackFrame.AddrPC.Offset);
		if(stackFrame.AddrPC.Offset == 0) {
			break;
		}
	}
}

LONG handleMemoryAccessViolation(EXCEPTION_POINTERS* exceptionInfo) {
	g_isMonitored = FALSE;
	Syelog(SYELOG_SEVERITY_INFORMATION, "Seting g_isMonitored to FALSE: %d\n", g_isMonitored);
	void* affectedAddress = (void*)exceptionInfo->ExceptionRecord->ExceptionInformation[1];
	DWORD oldProtect;
	BOOL virtualProtectResult = VirtualProtect(affectedAddress, 0x1000, PAGE_READWRITE, &oldProtect);
	
	Syelog(SYELOG_SEVERITY_DEBUG, "Result of VirtualProtect: %d\n",	virtualProtectResult);
	if(virtualProtectResult == 0) {
		Syelog(SYELOG_SEVERITY_DEBUG, "Result of error: %d\n",	GetLastError());
	}
	
	Syelog(SYELOG_SEVERITY_CRITICAL, "- ContextFlags: 0x%08X\n", exceptionInfo->ContextRecord->ContextFlags);
	Syelog(SYELOG_SEVERITY_CRITICAL, "- RIP: 0x%016lX\n", exceptionInfo->ContextRecord->Rip);
	Syelog(SYELOG_SEVERITY_CRITICAL, "- RSP: 0x%016lX\n", exceptionInfo->ContextRecord->Rsp);
	Syelog(SYELOG_SEVERITY_CRITICAL, "- RBP: 0x%016lX\n", exceptionInfo->ContextRecord->Rbp);
	Syelog(SYELOG_SEVERITY_CRITICAL, "- RAX: 0x%016lX\n", exceptionInfo->ContextRecord->Rax);
	Syelog(SYELOG_SEVERITY_CRITICAL, "- RBX: 0x%016lX\n", exceptionInfo->ContextRecord->Rbx);
	Syelog(SYELOG_SEVERITY_CRITICAL, "- RCX: 0x%016lX\n", exceptionInfo->ContextRecord->Rcx);
	Syelog(SYELOG_SEVERITY_CRITICAL, "- RDX: 0x%016lX\n", exceptionInfo->ContextRecord->Rdx);
	Syelog(SYELOG_SEVERITY_CRITICAL, "- RSI: 0x%016lX\n", exceptionInfo->ContextRecord->Rsi);
	Syelog(SYELOG_SEVERITY_CRITICAL, "- RDI: 0x%016lX\n", exceptionInfo->ContextRecord->Rdi);
	Syelog(SYELOG_SEVERITY_CRITICAL, "- affectedAddress: 0x%016p\n", affectedAddress);
	Syelog(SYELOG_SEVERITY_CRITICAL, "- affectedAddress value: 0x%d\n", *(BYTE*)affectedAddress);

	Syelog(SYELOG_SEVERITY_CRITICAL, "HANDLING VECTORED EXCEPTION! [Caller: %p] TYPE: %ld\n", _ReturnAddress(), exceptionInfo->ExceptionRecord->ExceptionCode);
	WalkStack(exceptionInfo->ContextRecord);
	Syelog(SYELOG_SEVERITY_CRITICAL, "StackWalk done - urmom\n");
	// g_isMonitored = TRUE;
	// VirtualProtect(affectedAddress, 1, PAGE_NOACCESS, &oldProtect);
	return EXCEPTION_CONTINUE_EXECUTION;
}

VOID* MyHeapAlloc(HANDLE hHeap, DWORD dwFlags,SIZE_T dwBytes) {
	Syelog(SYELOG_SEVERITY_INFORMATION, "g_isMonitored is: %d\n", g_isMonitored);
	VOID* ret = TrueHeapAlloc(hHeap, dwFlags, dwBytes);
	DWORD lpfOldProtect;
	Syelog(SYELOG_SEVERITY_INFORMATION, "HeapAlloc(0x%lx,0x%lx,0x%x) -> 0x%p\n", hHeap, dwFlags, dwBytes, ret);
	if(g_isMonitored == TRUE) {
		VirtualProtect(ret, dwBytes, PAGE_NOACCESS, &lpfOldProtect);
	} else {
		Syelog(SYELOG_SEVERITY_INFORMATION, "Protection is turned off!\n");
	}
	return ret;
};

BOOL MyHeapFree(HANDLE hHeap,DWORD dwFlags,VOID* lpMem) {
	BOOL ret = TrueHeapFree(hHeap,dwFlags,lpMem);
	Syelog(SYELOG_SEVERITY_INFORMATION, "HeapFree(0x%lx,0x%lx,0x%p) -> %x\n", hHeap, dwFlags, lpMem, ret);
	return ret;
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		AddVectoredExceptionHandler(1, handleMemoryAccessViolation);
		SyelogOpen("active_mem", SYELOG_FACILITY_APPLICATION);
		Syelog(SYELOG_SEVERITY_INFORMATION, "Hello world!\n");
        // MACRO_DETOURS_ATTACH(VirtualAlloc, MyVirtualAlloc);
		MACRO_DETOURS_ATTACH(HeapFree, MyHeapFree);
        MACRO_DETOURS_ATTACH(HeapAlloc, MyHeapAlloc);
		break;
	case DLL_PROCESS_DETACH:
		MACRO_DETOURS_DETACH(HeapAlloc, MyHeapAlloc);
        MACRO_DETOURS_DETACH(HeapFree, MyHeapFree);
		Syelog(SYELOG_SEVERITY_INFORMATION, "Goodbye world!\n");
		Sleep(1000);
		SyelogClose(FALSE);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}