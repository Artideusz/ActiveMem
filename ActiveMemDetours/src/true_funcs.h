#include <windows.h>

// VOID* MyHeapAlloc(HANDLE hHeap,DWORD dwFlags,SIZE_T dwBytes) {
//	return TrueHeapAlloc(hHeap,dwFlags,dwBytes);
// };
static VOID* (WINAPI* TrueHeapAlloc)(HANDLE hHeap,DWORD dwFlags,SIZE_T dwBytes) = HeapAlloc;

// BOOL MyHeapFree(HANDLE hHeap,DWORD dwFlags,VOID* lpMem) {
//	return TrueHeapFree(hHeap,dwFlags,lpMem);
// };
static BOOL (WINAPI* TrueHeapFree)(HANDLE hHeap,DWORD dwFlags,VOID* lpMem) = HeapFree;
