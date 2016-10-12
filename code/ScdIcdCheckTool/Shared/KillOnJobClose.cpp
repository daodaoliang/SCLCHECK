#include <Windows.h>

static class AutoKillChildProcessJob
{
public:
    AutoKillChildProcessJob()
        : m_hJob(NULL)
    {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli;

        ZeroMemory(&jeli, sizeof(jeli));

        m_hJob = CreateJobObject(NULL, NULL);
        jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE | JOB_OBJECT_LIMIT_BREAKAWAY_OK;
        SetInformationJobObject(m_hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
        AssignProcessToJobObject(m_hJob, GetCurrentProcess());
    }

    virtual ~AutoKillChildProcessJob()
    {
        if (m_hJob != NULL)
        {
            CloseHandle(m_hJob);
        }
    }

private:
    HANDLE m_hJob;

} g_job;
