#define MACRO_DETOURS_ATTACH(_FUNC, _MY_FUNC) {\
    DetourTransactionBegin(); \
    DetourUpdateThread(GetCurrentThread()); \
    DetourAttach(&(PVOID&)True ## _FUNC, _MY_FUNC); \
    if(DetourTransactionCommit() != NO_ERROR) \
        Syelog(SYELOG_SEVERITY_FATAL, #_FUNC" caused an error upon attaching"); \
}

#define MACRO_DETOURS_DETACH(_FUNC, _MY_FUNC) {\
    DetourTransactionBegin(); \
    DetourUpdateThread(GetCurrentThread()); \
    DetourDetach(&(PVOID&)True ## _FUNC, _MY_FUNC); \
    if(DetourTransactionCommit() != NO_ERROR) \
        Syelog(SYELOG_SEVERITY_FATAL, #_FUNC" caused an error upon detaching: "); \
}