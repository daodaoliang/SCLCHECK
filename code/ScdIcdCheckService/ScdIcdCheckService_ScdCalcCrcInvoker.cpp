#include "ScdIcdCheckService_ScdCalcCrcInvoker.h"
#include <Shlwapi.h>
#include <ShlObj.h>
#include "../ScdCheckCommon/charconv.h"
#include <strsafe.h>
using namespace std;

CScdIcdCheckService_ScdCalcCrcInvoker::CScdIcdCheckService_ScdCalcCrcInvoker()
{
    if (IsSystem64bit() && PathFileExistsA("x64\\ScdCrcCalculator264.dll"))
    {
        m_strCalcCrcDllPath = "x64\\ScdCrcCalculator264.dll";
    }
    else
    {
        m_strCalcCrcDllPath = "ScdCrcCalculator2.dll";
    }
}

CScdIcdCheckService_ScdCalcCrcInvoker::~CScdIcdCheckService_ScdCalcCrcInvoker()
{
}

std::string CScdIcdCheckService_ScdCalcCrcInvoker::InvokeScdCalcCrc(const std::string &strFilePath, const std::string &strResultStorePath) const
{
    // 判断文件是否存在
    if (!PathFileExistsA(strFilePath.c_str()))
    {
        return MakeResultJson(false, "文件%hs不存在", strFilePath.c_str());
    }

    // 构造文件路径
    WCHAR szResultStorePath[MAX_PATH];
    WCHAR szResultFilePath[MAX_PATH];
    WCHAR szProgressFilePath[MAX_PATH];

    StringCchPrintfW(szResultStorePath, RTL_NUMBER_OF(szResultStorePath), L"%hs", strResultStorePath.c_str());
    PathAddBackslashW(szResultStorePath);

    StringCchPrintfW(szResultFilePath, RTL_NUMBER_OF(szResultFilePath), L"%lsresult.xml", szResultStorePath);
    StringCchPrintfW(szProgressFilePath, RTL_NUMBER_OF(szProgressFilePath), L"%lsprogress.txt", szResultStorePath);

    // 创建目录
    SHCreateDirectory(NULL, szResultStorePath);

    // 判断目录创建是否成功
    WIN32_FILE_ATTRIBUTE_DATA wfad;

    if (!GetFileAttributesExW(szResultStorePath, GetFileExInfoStandard, &wfad) ||
        (wfad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
    {
        return MakeResultJson(false, "结果存放目录不存在且无法创建%lu", GetLastError());
    }

    // 启动工作进程
    TCHAR szCmd[MAX_PATH * 4 + 1024];
    STARTUPINFO si = {sizeof(si)};
    PROCESS_INFORMATION pi;

    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    StringCchPrintf(
        szCmd,
        RTL_NUMBER_OF(szCmd),
        TEXT("rundll32.exe \"%hs\" CalcCrcByRundll32 \"%hs\" \"%hs\""),
        m_strCalcCrcDllPath.c_str(),
        strFilePath.c_str(),
        strResultStorePath.c_str());
    SafeDebugMessageA("启动工作进程111：%ls\r\n", szCmd);

    if (CreateProcess(NULL, szCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
        if (WaitForSingleObject(pi.hProcess, 20 * 60 * 1000) == WAIT_TIMEOUT)
        {
            TerminateProcess(pi.hProcess, 58);
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if (PathFileExistsW(szResultFilePath))
        {
            return MakeResultJson(true, NULL);
        }
        else
        {
            return MakeResultJson(false, "出现错误，请查看progress.txt");
        }
    }
    else
    {
        return MakeResultJson(false, "启动工作进程失败%lu", GetLastError());
    }
}
