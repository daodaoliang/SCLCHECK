#ifndef _RUNNER_H
#define _RUNNER_H

#include <Windows.h>
#pragma warning(disable: 4786)
#include <memory>
#include <string>
#include <vector>

#include <Shlwapi.h>
#include <list>
#include <sstream>
#include "Common.h"
#include "charconv.h"

#pragma comment(lib, "Shlwapi.lib")

using namespace std;

#define CloseIfNotNull(h) do {      \
            if ((h) != NULL) {      \
                CloseHandle((h));   \
                (h) = NULL;         \
            }                       \
} while (false)


// �������ݵĶ���(���ݶ�ȡ�߳����������߳�)
struct CheckInfo
{
    wstring position;
    wstring sKey;
    wstring sName;
    wstring sDesc;
    wstring sStandard;
    wstring sFlag;
    wstring sClass;
    wstring sXml;
};

typedef vector<CheckInfo> CheckResults;

class RunnerImpl
{
    class CScopedCriticalSection
    {
    public:
        CScopedCriticalSection(LPCRITICAL_SECTION lpCs)
            : m_lpCs(lpCs)
        {
            EnterCriticalSection(m_lpCs);
        }

        ~CScopedCriticalSection()
        {
            LeaveCriticalSection(m_lpCs);
        }

    private:
        LPCRITICAL_SECTION m_lpCs;
    };

public:
    RunnerImpl(LPCTSTR lpCmd)
        : m_hProcess(NULL)
        , m_hMainThread(NULL)
        , m_hInRead(NULL)
        , m_hInWrite(NULL)
        , m_hOutRead(NULL)
        , m_hOutWrite(NULL)
        , m_hReadThread(NULL)
        , m_hStopReadThreadEvent(NULL)
    {
        SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
        TCHAR szCmd[4096] = TEXT("");

        if (lpCmd != NULL)
        {
            lstrcpyn(szCmd, lpCmd, RTL_NUMBER_OF(szCmd));
        }

        InitializeCriticalSection(&m_csRead);

         CreatePipe(&m_hInRead, &m_hInWrite, &sa, 1024*1024);
         CreatePipe(&m_hOutRead, &m_hOutWrite, &sa, 1024*1024);

        DWORD dw = GetLastError();

        STARTUPINFO si = {sizeof(si)};
        PROCESS_INFORMATION pi;

        si.dwFlags = STARTF_USESTDHANDLES/* | STARTF_USESHOWWINDOW*/;
        si.hStdInput = m_hInRead;
        si.hStdOutput = m_hOutWrite;
        si.wShowWindow = SW_HIDE;

        SafeDebugMessageA("�������̣� %s", WtoA(szCmd).c_str());
        if (CreateProcess(NULL, szCmd, NULL, NULL, TRUE, DETACHED_PROCESS, NULL, NULL, &si, &pi))
        {
            m_hProcess = pi.hProcess;
            m_hMainThread = pi.hThread;

            m_hStopReadThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
            m_hReadThread = CreateThread(NULL, 0, ReadProc, this, 0, NULL);
        }
        else
        {
            DWORD dwLastError = GetLastError();
            TCHAR szErrorMessage[1024];

            wnsprintf(szErrorMessage, RTL_NUMBER_OF(szErrorMessage), TEXT("Runner ��������(%s)ʧ�ܣ�������%lu\r\n"), szCmd, GetLastError());
            OutputDebugString(szErrorMessage);
        }
    }

    ~RunnerImpl()
    {
        if (m_hReadThread != NULL)
        {
            SetEvent(m_hStopReadThreadEvent);

            WaitForSingleObject(m_hStopReadThreadEvent, 1001);

            if (IsThreadRunning(m_hReadThread))
            {
                TerminateThread(m_hReadThread, 0);
                Sleep(19);
            }
        }

        CloseIfNotNull(m_hProcess);
        CloseIfNotNull(m_hMainThread);
        CloseIfNotNull(m_hInRead);
        CloseIfNotNull(m_hInWrite);
        CloseIfNotNull(m_hOutRead);
        CloseIfNotNull(m_hOutWrite);
        CloseIfNotNull(m_hReadThread);
        CloseIfNotNull(m_hStopReadThreadEvent);
        DeleteCriticalSection(&m_csRead);
    }

    DWORD Join(DWORD dwTimeout = INFINITE) const
    {
        if (GetProcess() == NULL)
        {
            return WAIT_OBJECT_0;
        }

        HANDLE hObjects[] = {GetProcess(), m_hReadThread};

        return WaitForMultipleObjects(RTL_NUMBER_OF(hObjects), hObjects, TRUE, dwTimeout);
    }

    BOOL SendBuffer(const char *lpBuffer, DWORD dwSize)
    {
        if (!IsRunning())
        {
            return FALSE;
        }

        return WriteFileHelper(m_hInWrite, lpBuffer, dwSize);
    }

    BOOL SendLine(const char *lpText)
    {
        int length = lstrlenA(lpText);

        if (length > 0)
        {
            if (!SendBuffer(lpText, length))
            {
                return FALSE;
            }
        }

        if (length == 0 || lpText[length - 1] != '\n')
        {
            return SendBuffer("\n", 1);
        }

        return FALSE;
    }

    BOOL GetLine(std::string &strLine)
    {
        CScopedCriticalSection __cs(&m_csRead);
        list<string>::size_type nSize = m_listLines.size();

        if (nSize == 1)
        {
            m_bLastLineCompleted = true;
        }

        if (nSize > 0)
        {
            strLine = m_listLines.front();
            m_listLines.pop_front();

            return TRUE;
        }

        return FALSE;
    }

    BOOL GetAllLines(std::vector<std::string> &vectorAllLines)
    {
        CScopedCriticalSection __cs(&m_csRead);
        list<string>::size_type nSize = m_listLines.size();

        if (!m_listLines.empty())
        {
            vectorAllLines.reserve(m_listLines.size());
#if _MSC_VER > 1200
            vectorAllLines.assign(m_listLines.begin(), m_listLines.end());
#else
            for (list<string>::const_iterator it = m_listLines.begin(); it != m_listLines.end(); ++it)
            {
                vectorAllLines.push_back(*it);
            }
#endif
            m_listLines.clear();

            return TRUE;
        }

        return FALSE;
    }

    DWORD GetExitCode() const
    {
        DWORD dwExitCode = -1;

        if (GetProcess() != NULL)
        {
            GetExitCodeProcess(GetProcess(), &dwExitCode);
        }

        return dwExitCode;
    }

    BOOL IsRunning() const
    {
        return GetExitCode() == STILL_ACTIVE;
    }

    HANDLE GetProcess() const
    {
        return m_hProcess;
    }
    // �������
    virtual bool ProcessData(const std::string& sData)
    {
        return false;
    }
    // �����߳̽���
    virtual bool ProcessOver()
    {
        return false;
    }

private:
    static BOOL ReadAllDataInThread(RunnerImpl *pRunnerImpl)
    {
        stringstream ss;
        bool bDataRead = false;

        // ȡ����������
        while (true)
        {
            char szBuffer[4096];
            DWORD dwBytesRead = 0;
            DWORD dwBytesAvail = 0;

            PeekNamedPipe(pRunnerImpl->m_hOutRead, NULL, 0, NULL, &dwBytesAvail, NULL);

            if (dwBytesAvail == 0)
            {
                break;
            }

            ReadFile(pRunnerImpl->m_hOutRead, szBuffer, RTL_NUMBER_OF(szBuffer), &dwBytesRead, NULL);

            if (dwBytesRead == 0)
            {
                break;
            }

            ss<<string(szBuffer, dwBytesRead);
            bDataRead = true;
        }

        // ��������ݣ������ȴ�
        if (!bDataRead)
        {
            return FALSE;
        }

        // ��ȡ����Ϊ�ַ�������ȷ��������
        string strBuffer = ss.str();

        if (strBuffer.empty())
        {
            return FALSE;
        }
        Replace(strBuffer, string("\r"), string(""));
        // ���в�����ݣ����Կհ���
        vector<string> vectorLines;
        char chDelim = '>';

        bool bEndWithCrLf = *strBuffer.rbegin() == chDelim;

        SplitString(strBuffer, string(&chDelim,1), vectorLines, true);

        vector<string>::iterator it = vectorLines.begin();
        // �����һ�ε����һ��δ��ɣ������֮
        if (!pRunnerImpl->m_bLastLineCompleted)
        {
            *it = pRunnerImpl->m_sLastRemain + it->c_str();
            pRunnerImpl->m_sLastRemain.clear();
        }
        // ȥ��δ��β����
        if (!bEndWithCrLf)
        {
            pRunnerImpl->m_sLastRemain = *vectorLines.rbegin();
            vectorLines.erase(vectorLines.end()-1);
        }
        // �����ֳ�������һ�л���û�в�ֳ��κ�һ�е����Իس�����������
        // ����Ӱ��m_bLastLineCompleted�ķ�գ��ż�������
        if (vectorLines.empty() && bEndWithCrLf)
        {
            return FALSE;
        }
        else
        {
            CScopedCriticalSection __cs(&pRunnerImpl->m_csRead);

            // �������һ����¼
            if (vectorLines.empty() && bEndWithCrLf && !pRunnerImpl->m_bLastLineCompleted)
            {
                pRunnerImpl->m_bLastLineCompleted = true;
            }

            // ׷��
            if (!vectorLines.empty())
            {
                vector<string>::iterator it = vectorLines.begin();
                // Ϥ��׷��
                for (; it != vectorLines.end(); ++it)
                {
                    if (!pRunnerImpl->ProcessData(*it))
                    {
                        pRunnerImpl->m_listLines.push_back(*it);
                    }
                }
            }

            // ��¼��һ��line�Ƿ������
            pRunnerImpl->m_bLastLineCompleted = bEndWithCrLf;

            // ���سɹ�
            return TRUE;
        }
    }

    //  ��ȡ�̣߳����캯�����ѱ�֤���̾����Ч
    static DWORD CALLBACK ReadProc(LPVOID lpParam)
    {
        RunnerImpl *pRunnerImpl = (RunnerImpl *)lpParam;
        string strBuffer;
        HANDLE hObjects[] = {pRunnerImpl->m_hStopReadThreadEvent, pRunnerImpl->m_hProcess}; //  ���Ķ�·

        while (true)
        {
            DWORD dwWaitResult = WaitForMultipleObjects(RTL_NUMBER_OF(hObjects), hObjects, FALSE, 100);

            ReadAllDataInThread(pRunnerImpl);

            if (dwWaitResult == WAIT_OBJECT_0 || dwWaitResult == WAIT_OBJECT_0 + 1 || dwWaitResult == WAIT_FAILED)
            {
                break;
            }
        }
        pRunnerImpl->ProcessOver();
        return 0;
    }

    static BOOL IsThreadRunning(HANDLE hThread)
    {
        DWORD dwExitCode = 0;

        if (hThread != NULL)
        {
            GetExitCodeProcess(hThread, &dwExitCode);
        }

        return dwExitCode == STILL_ACTIVE;
    }

    static BOOL WriteFileHelper(HANDLE hFile, LPCVOID lpBuffer, DWORD dwBytesToWtite)
    {
        while (dwBytesToWtite > 0)
        {
            DWORD dwBytesWritten = 0;

            if (!WriteFile(hFile, lpBuffer, dwBytesToWtite, &dwBytesWritten, NULL))
            {
                return FALSE;
            }

            dwBytesToWtite -= dwBytesWritten;
            (LPCSTR&)lpBuffer += dwBytesWritten;
        }

        return TRUE;
    }

    static BOOL WriteLine(HANDLE hPipe, const char *line)
    {
        int length = lstrlenA(line);

        if (length > 0)
        {
            if (!WriteFileHelper(hPipe, line, length))
            {
                return FALSE;
            }
        }

        if (length == 0 || line[length - 1] != '\n')
        {
            if (!WriteFileHelper(hPipe, "\n", 1))
            {
                return FALSE;
            }
        }

        return TRUE;
    }

    // ���ܣ���pattenΪ�ָ������ַ���str�ָ�������result�У�������result�Ĵ�С��ignore_blank��ʶ�Ƿ���Կհײ���
    // ע�⣺ֻҪ���������ã�result�������ȱ��ÿ�
    // ע�⣺ԭʼ�ַ�����β�Ŀհ�ʼ�ղ�������result��
    // ���磺��*�ָ���a*b���͡�*a*b���͡�a*b*���͡�*a*b*��ʱ����ignore_blankΪ�εõ��Ľ������������
    template <class T>
    static size_t SplitString(const std::basic_string<T> &str, const std::basic_string<T> &patten, std::vector<std::basic_string<T> > &result, bool ignore_blank = true)
    {
        std::basic_string<T>::size_type pos = str.find(patten);
        std::basic_string<T>::size_type begin = 0;
        bool first = true;

        result.clear();

        while (true)
        {
            if (pos == str.npos)
            {
                if (begin < str.size())
                {
                    result.push_back(str.substr(begin));
                }

                return result.size();
            }
            else
            {
                if (pos > begin || (!first && !ignore_blank))
                {
                    result.push_back(str.substr(begin, pos - begin));
                }

                begin = pos + patten.size();
                pos = str.find(patten, begin);

                first = false;
            }
        }
    }

    template <class T>
    static void Replace(std::basic_string<T> &str, const std::basic_string<T> &old_str, const std::basic_string<T> &new_str)
    {
        std::basic_string<T>::size_type offset = str.find(old_str);

        while (offset != str.npos)
        {
            str.replace(offset, old_str.length(), new_str);

            offset = str.find(old_str, offset + new_str.length());
        }
    }

private:
    HANDLE m_hProcess;                 // �ӽ��̾��
    HANDLE m_hMainThread;              // �ӽ��̵����߳�
    HANDLE m_hInRead;                  // �ӽ��̵�StdIn�Ķ�ȡ��
    HANDLE m_hInWrite;                 // �ӽ��̵�StdIn��д���
    HANDLE m_hOutRead;                 // �ӽ��̵�StdOut��StdError�Ķ�ȡ��
    HANDLE m_hOutWrite;                // �ӽ��̵�StdOut��StdError��д���
    HANDLE m_hReadThread;              // ��ȡ�߳�
    HANDLE m_hStopReadThreadEvent;     // ֹͣ��ȡ�߳��¼�
    CRITICAL_SECTION m_csRead;         // �ٽ���
    list<string> m_listLines;          // ���е�lines
    bool m_bLastLineCompleted;         // ���һ�������Ƿ��Ѿ��յ����Լ��Ļس���
    string m_sLastRemain;              // ��һ�β�����δ��β������
};


#endif /* _RUNNDER_H */