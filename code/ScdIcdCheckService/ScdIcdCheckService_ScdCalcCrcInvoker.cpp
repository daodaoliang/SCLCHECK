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
    // �ж��ļ��Ƿ����
    if (!PathFileExistsA(strFilePath.c_str()))
    {
        return MakeResultJson(false, "�ļ�%hs������", strFilePath.c_str());
    }

    // �����ļ�·��
    WCHAR szResultStorePath[MAX_PATH];
    WCHAR szResultFilePath[MAX_PATH];
    WCHAR szProgressFilePath[MAX_PATH];

    StringCchPrintfW(szResultStorePath, RTL_NUMBER_OF(szResultStorePath), L"%hs", strResultStorePath.c_str());
    PathAddBackslashW(szResultStorePath);

    StringCchPrintfW(szResultFilePath, RTL_NUMBER_OF(szResultFilePath), L"%lsresult.xml", szResultStorePath);
    StringCchPrintfW(szProgressFilePath, RTL_NUMBER_OF(szProgressFilePath), L"%lsprogress.txt", szResultStorePath);

    // ����Ŀ¼
    SHCreateDirectory(NULL, szResultStorePath);

    // �ж�Ŀ¼�����Ƿ�ɹ�
    WIN32_FILE_ATTRIBUTE_DATA wfad;

    if (!GetFileAttributesExW(szResultStorePath, GetFileExInfoStandard, &wfad) ||
        (wfad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
    {
        return MakeResultJson(false, "������Ŀ¼���������޷�����%lu", GetLastError());
    }

    // ������������
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
    SafeDebugMessageA("������������111��%ls\r\n", szCmd);

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
            return MakeResultJson(false, "���ִ�����鿴progress.txt");
        }
    }
    else
    {
        return MakeResultJson(false, "������������ʧ��%lu", GetLastError());
    }
}
