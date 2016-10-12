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


// 共享数据的定义(数据读取线程与主界面线程)
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

        SafeDebugMessageA("启动进程： %s", WtoA(szCmd).c_str());
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

            wnsprintf(szErrorMessage, RTL_NUMBER_OF(szErrorMessage), TEXT("Runner 创建进程(%s)失败，错误码%lu\r\n"), szCmd, GetLastError());
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
    // 处理过程
    virtual bool ProcessData(const std::string& sData)
    {
        return false;
    }
    // 处理线程结束
    virtual bool ProcessOver()
    {
        return false;
    }

private:
    static BOOL ReadAllDataInThread(RunnerImpl *pRunnerImpl)
    {
        stringstream ss;
        bool bDataRead = false;

        // 取出所有数据
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

        // 如果无数据，继续等待
        if (!bDataRead)
        {
            return FALSE;
        }

        // 提取数据为字符串，并确保有数据
        string strBuffer = ss.str();

        if (strBuffer.empty())
        {
            return FALSE;
        }
        Replace(strBuffer, string("\r"), string(""));
        // 按行拆分数据，忽略空白行
        vector<string> vectorLines;
        char chDelim = '>';

        bool bEndWithCrLf = *strBuffer.rbegin() == chDelim;

        SplitString(strBuffer, string(&chDelim,1), vectorLines, true);

        vector<string>::iterator it = vectorLines.begin();
        // 如果上一次的最后一行未完成，则完成之
        if (!pRunnerImpl->m_bLastLineCompleted)
        {
            *it = pRunnerImpl->m_sLastRemain + it->c_str();
            pRunnerImpl->m_sLastRemain.clear();
        }
        // 去除未结尾内容
        if (!bEndWithCrLf)
        {
            pRunnerImpl->m_sLastRemain = *vectorLines.rbegin();
            vectorLines.erase(vectorLines.end()-1);
        }
        // 如果拆分出了至少一行或者没有拆分出任何一行但是以回车结束（后者
        // 可能影响m_bLastLineCompleted的封闭）才继续处理
        if (vectorLines.empty() && bEndWithCrLf)
        {
            return FALSE;
        }
        else
        {
            CScopedCriticalSection __cs(&pRunnerImpl->m_csRead);

            // 仅封闭上一条记录
            if (vectorLines.empty() && bEndWithCrLf && !pRunnerImpl->m_bLastLineCompleted)
            {
                pRunnerImpl->m_bLastLineCompleted = true;
            }

            // 追加
            if (!vectorLines.empty())
            {
                vector<string>::iterator it = vectorLines.begin();
                // 悉数追加
                for (; it != vectorLines.end(); ++it)
                {
                    if (!pRunnerImpl->ProcessData(*it))
                    {
                        pRunnerImpl->m_listLines.push_back(*it);
                    }
                }
            }

            // 记录上一个line是否已完成
            pRunnerImpl->m_bLastLineCompleted = bEndWithCrLf;

            // 返回成功
            return TRUE;
        }
    }

    //  读取线程，构造函数中已保证进程句柄有效
    static DWORD CALLBACK ReadProc(LPVOID lpParam)
    {
        RunnerImpl *pRunnerImpl = (RunnerImpl *)lpParam;
        string strBuffer;
        HANDLE hObjects[] = {pRunnerImpl->m_hStopReadThreadEvent, pRunnerImpl->m_hProcess}; //  当心短路

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

    // 功能：以patten为分隔符将字符串str分隔并置入result中，并返回result的大小，ignore_blank标识是否忽略空白部分
    // 注意：只要函数被调用，result都将首先被置空
    // 注意：原始字符串首尾的空白始终不被置入result中
    // 例如：以*分隔“a*b”和“*a*b”和“a*b*”和“*a*b*”时无论ignore_blank为何得到的结果总是两部分
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
    HANDLE m_hProcess;                 // 子进程句柄
    HANDLE m_hMainThread;              // 子进程的主线程
    HANDLE m_hInRead;                  // 子进程的StdIn的读取端
    HANDLE m_hInWrite;                 // 子进程的StdIn的写入端
    HANDLE m_hOutRead;                 // 子进程的StdOut，StdError的读取端
    HANDLE m_hOutWrite;                // 子进程的StdOut，StdError的写入端
    HANDLE m_hReadThread;              // 读取线程
    HANDLE m_hStopReadThreadEvent;     // 停止读取线程事件
    CRITICAL_SECTION m_csRead;         // 临界区
    list<string> m_listLines;          // 所有的lines
    bool m_bLastLineCompleted;         // 最后一行数据是否已经收到了自己的回车符
    string m_sLastRemain;              // 上一次残留的未结尾行内容
};


#endif /* _RUNNDER_H */