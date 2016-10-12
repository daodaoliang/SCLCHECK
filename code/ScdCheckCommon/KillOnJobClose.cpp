#include <Windows.h>
#include "KillOnJobClose.h"

void SetKillOnJobClose()
{
    BOOL bProcessInJob = FALSE;
    BOOL (WINAPI *pIsProcessInJob)(
        __in          HANDLE ProcessHandle,
        __in          HANDLE JobHandle,
        __out         PBOOL Result
        ) = NULL;

    (PROC &)pIsProcessInJob = GetProcAddress(GetModuleHandle(TEXT("Kernel32.dll")), "IsProcessInJob");

    if (pIsProcessInJob &&
        pIsProcessInJob(GetCurrentProcess(), NULL, &bProcessInJob) &&
        bProcessInJob)
    {
        return;
    }

    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli;
    HANDLE hJobObject = CreateJobObject(NULL, NULL);

    ZeroMemory(&jeli, sizeof(jeli));
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE | JOB_OBJECT_LIMIT_BREAKAWAY_OK;

    SetInformationJobObject(hJobObject, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
    AssignProcessToJobObject(hJobObject, GetCurrentProcess());
}
