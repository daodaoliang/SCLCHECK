#include "ScdIcdCheckService_CheckerInvoker.h"
#include "../ScdCheckCommon/charconv.h"
#include "../ScdCheckCommon/Common.h"
#include <Windows.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#include <fstream>
#include <stdarg.h>
#include <boost/scope_exit.hpp>
#include "../ScdCheckCommon/charconv.h"
#include <strsafe.h>
using namespace std;

CScdIcdCheckService_CheckerInvoker::CScdIcdCheckService_CheckerInvoker(void)
{
    if (IsSystem64bit() && PathFileExistsA("x64\\ScdChecker64.exe"))
    {
        m_strScdCheckerPath = "x64\\ScdChecker64.exe";
    }
    else
    {
        m_strScdCheckerPath = "ScdChecker.exe";
    }

    if (IsSystem64bit() && PathFileExistsA("x64\\IcdChecker64.exe"))
    {
        m_strIcdCheckerPath = "x64\\IcdChecker64.exe";
    }
    else
    {
        m_strIcdCheckerPath = "IcdChecker.exe";
    }

    if (IsSystem64bit() && PathFileExistsA("x64\\Xml2Xsl64.exe"))
    {
        m_strXml2XslPath = "x64\\Xml2Xsl64.exe";
    }
    else
    {
        m_strXml2XslPath = "Xml2Xsl.exe";
    }
}

CScdIcdCheckService_CheckerInvoker::~CScdIcdCheckService_CheckerInvoker(void)
{
}

std::strutf8 CScdIcdCheckService_CheckerInvoker::InvokeScdChecker(const std::string &strConfigFilePath, const std::string &strFilePath, const std::string &strResultStorePath) const
{
    return InvodeChecker(m_strScdCheckerPath, strConfigFilePath, strFilePath, strResultStorePath);
}

std::strutf8 CScdIcdCheckService_CheckerInvoker::InvokeIcdChecker(const std::string &strConfigFilePath, const std::string &strFilePath, const std::string &strResultStorePath) const
{
    return InvodeChecker(m_strIcdCheckerPath, strConfigFilePath, strFilePath, strResultStorePath);
}

std::strutf8 CScdIcdCheckService_CheckerInvoker::InvodeChecker(
    const std::string &strCheckerPath,
    const std::string &strConfigFilePath,
    const std::string &strFilePath,
    const std::string &strResultStorePath) const
{
    // 构造文件路径
    WCHAR szResultStorePath[MAX_PATH];
    WCHAR szResultFilePath[MAX_PATH];
    WCHAR szProgressFilePath[MAX_PATH];

    StringCchPrintf(szResultStorePath, RTL_NUMBER_OF(szResultStorePath), L"%hs", strResultStorePath.c_str());
    PathAddBackslashW(szResultStorePath);

    StringCchPrintf(szResultFilePath, RTL_NUMBER_OF(szResultFilePath), L"%lsresult.xml", szResultStorePath);
    StringCchPrintf(szProgressFilePath, RTL_NUMBER_OF(szProgressFilePath), L"%lsprogress.txt", szResultStorePath);

    // 创建目录
    SHCreateDirectory(NULL, szResultStorePath);

    // 判断目录创建是否成功
    WIN32_FILE_ATTRIBUTE_DATA wfad;

    if (!GetFileAttributesExW(szResultStorePath, GetFileExInfoStandard, &wfad) ||
        (wfad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
    {
        return MakeResultJson(false, "结果存放目录不存在且无法创建%lu", GetLastError());
    }

    // 创建输出文件
    HANDLE hResultFile = INVALID_HANDLE_VALUE;
    HANDLE hProgressFile = INVALID_HANDLE_VALUE;

    BOOST_SCOPE_EXIT((&hResultFile)(&hProgressFile))
    {
        if (hResultFile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hResultFile);
        }

        if (hProgressFile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hProgressFile);
        }
    }
    BOOST_SCOPE_EXIT_END;

    hResultFile = CreateFileW(szResultFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hResultFile != INVALID_HANDLE_VALUE)
    {
        SetHandleInformation(hResultFile, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
    }
    else
    {
        return MakeResultJson(false, "result.xml无法被创建%lu", GetLastError());
    }

    hProgressFile = CreateFileW(szProgressFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hProgressFile != INVALID_HANDLE_VALUE)
    {
        SetHandleInformation(hProgressFile, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
    }
    else
    {
        return MakeResultJson(false, "progress.txt无法被创建%lu", GetLastError());
    }

    // 启动工作进程
    TCHAR szCmd[MAX_PATH * 4 + 1024];
    STARTUPINFO si = {sizeof(si)};
    PROCESS_INFORMATION pi;

    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdOutput = hResultFile;
    si.hStdError = hProgressFile;

    StringCchPrintf(
        szCmd,
        RTL_NUMBER_OF(szCmd),
        TEXT("\"%hs\" \"%hs\" \"%hs\" \"%hs\""),
        strCheckerPath.c_str(),
        strConfigFilePath.c_str(),
        strFilePath.c_str(),
        "utf8");
    SafeDebugMessageA("启动工作进程111：%ls\r\n", szCmd);

    if (CreateProcess(NULL, szCmd, NULL, NULL, TRUE, 0, NULL, WtoT(szResultStorePath).c_str(), &si, &pi))
    {
        if (WaitForSingleObject(pi.hProcess, 20 * 60 * 1000) == WAIT_TIMEOUT)
        {
            TerminateProcess(pi.hProcess, 58);
        }

        DWORD dwExitCode = -1;

        FlushFileBuffers(hResultFile);
        FlushFileBuffers(hProgressFile);

        GetExitCodeProcess(pi.hProcess, &dwExitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        CloseHandle(hResultFile);
        CloseHandle(hProgressFile);
        hResultFile = INVALID_HANDLE_VALUE;
        hProgressFile = INVALID_HANDLE_VALUE;

        if (dwExitCode == 0)
        {
            ConvertResultXmlToRestltXslx(strFilePath, WtoA(szResultFilePath), strResultStorePath + "\\result.xls");
            return MakeResultJson(true, NULL);
        }
        else
        {
            return MakeResultJson(false, "工作进程返回错误码%d", (int)dwExitCode);
        }
    }
    else
    {
        return MakeResultJson(false, "启动工作进程失败%lu", GetLastError());
    }
}

bool CScdIcdCheckService_CheckerInvoker::ConvertResultXmlToRestltXslx(
    const std::string &strFilePath,
    const std::string &strResultXmlFilePath,
    const std::string &strResultXlsxFilePath) const
{
    bool bSucceed = false;
    WCHAR szCmd[MAX_PATH * 3 + 1024];
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    WCHAR szCurrentDirectory[MAX_PATH];

    GetModuleFileNameW(GetModuleHandle(NULL), szCurrentDirectory, RTL_NUMBER_OF(szCurrentDirectory));
    PathRemoveFileSpec(szCurrentDirectory);

    StringCchPrintf(
        szCmd,
        RTL_NUMBER_OF(szCmd),
        L"\"%hs\" \"-i%hs\" \"-o%hs\" \"-I%hs\"",
        m_strXml2XslPath.c_str(),
        strResultXmlFilePath.c_str(),
        strResultXlsxFilePath.c_str(),
        strFilePath.c_str());

    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    if (CreateProcessW(NULL, szCmd, NULL, NULL, FALSE, 0, NULL, szCurrentDirectory, &si, &pi))
    {
        if (WaitForSingleObject(pi.hProcess, 60 * 1000) == WAIT_TIMEOUT)
        {
            TerminateProcess(pi.hProcess, 58);
        }

        DWORD dwExitCodeProcess = 0;
        
        GetExitCodeProcess(pi.hProcess, &dwExitCodeProcess);
        bSucceed = dwExitCodeProcess == 0;

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    return bSucceed;
}
