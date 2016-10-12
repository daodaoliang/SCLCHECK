#include "../gsoap/soapScdIcdCheckerService.h"
#include "ScdIcdCheckService_CheckerInvoker.h"
#include "ScdIcdCheckService_ScdCalcCrcInvoker.h"
#include <Shlwapi.h>
#include <ShlObj.h>
#include <strsafe.h>
#include "../ScdCheckCommon/SsytemLux.h"

struct CRCStruct
{
    std::string strFilePath;
    std::string strResultStorePath;
    std::string strResultJson;
};

DWORD WINAPI InvokeScdCalcCrc(LPVOID strCRC)
{
    CRCStruct *crc = (CRCStruct*)strCRC;
    std::string strFilePath = crc->strFilePath;
    std::string strResultStorePath = crc->strResultStorePath;
    // �ж��ļ��Ƿ����
    if (!PathFileExistsA(strFilePath.c_str()))
    {
        crc->strResultJson = MakeResultJson(false, "�ļ�%hs������", strFilePath.c_str());
        return 1001;
        // return MakeResultJson(false, "�ļ�%hs������", strFilePath.c_str());
    }

    // �����ļ�·��
    WCHAR szResultStorePath[MAX_PATH];
    WCHAR szResultFilePath[MAX_PATH];
    WCHAR szProgressFilePath[MAX_PATH];
    std::string m_strCalcCrcDllPath;

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
        crc->strResultJson = MakeResultJson(false, "������Ŀ¼���������޷�����%lu", GetLastError());
        return 1002;
        // return MakeResultJson(false, "������Ŀ¼���������޷�����%lu", GetLastError());
    }

    if (IsSystem64bit() && PathFileExistsA("x64\\ScdCrcCalculator264.dll"))
    {
        m_strCalcCrcDllPath = "x64\\ScdCrcCalculator264.dll";
    }
    else
    {
        m_strCalcCrcDllPath = "ScdCrcCalculator2.dll";
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
            crc->strResultJson = MakeResultJson(true, NULL);
            return 1003;
            // return MakeResultJson(true, NULL);
        }
        else
        {
            crc->strResultJson = MakeResultJson(false, "���ִ�����鿴progress.txt");
            return 1004;
            // return MakeResultJson(false, "���ִ�����鿴progress.txt");
        }
    }
    else
    {
        crc->strResultJson = MakeResultJson(false, "������������ʧ��%lu", GetLastError());
        return 1005;
        // return MakeResultJson(false, "������������ʧ��%lu", GetLastError());
    }
}

int ScdIcdCheckerService::ScdCheck(std::string strConfigFilePath, std::string strFilePath, std::string strResultStorePath, std::string &strResultJson)
{
    //�жϲ���ϵͳ���ͣ���linuxĿ¼�ṹת����windowsĿ¼�ṹ
    if (g_System->IsRunningOnWine())
    {
        strConfigFilePath =  g_System->replace_all_distinct(strConfigFilePath,"/","\\");
        string ss = "Z:";
        ss.append(strConfigFilePath);
        strConfigFilePath =ss;

        strFilePath =  g_System->replace_all_distinct(strFilePath,"/","\\");
        string ss2 = "Z:";
        ss2.append(strFilePath);
        strFilePath =ss2;

        strResultStorePath =  g_System->replace_all_distinct(strResultStorePath,"/","\\");
        string ss3 = "Z:";
        ss3.append(strResultStorePath);
        strResultStorePath =ss3;
    }

    std::string oldstr = "/";
    std::string newstr = "\\";
    Replace(strConfigFilePath, oldstr, newstr);
    Replace(strFilePath, oldstr, newstr);
    Replace(strResultStorePath, oldstr, newstr);

    char resultPath[MAX_PATH];
    strcpy(resultPath, strResultStorePath.c_str());
    PathAppendA(resultPath, "CRC");
    
    CRCStruct tempCrcParam;
    tempCrcParam.strFilePath = strFilePath;
    tempCrcParam.strResultStorePath = resultPath;
    HANDLE handCrc = CreateThread(NULL, 0, InvokeScdCalcCrc, (LPVOID)(&tempCrcParam), 0, NULL);
    
    strResultJson = CScdIcdCheckService_CheckerInvoker::GetInstance()->InvokeScdChecker(strConfigFilePath, strFilePath, strResultStorePath);

    if (WaitForSingleObject(handCrc, 20 * 60 * 1000) == WAIT_TIMEOUT)
    {
        TerminateThread(handCrc, 59);
    }
    return SOAP_OK;
}

int ScdIcdCheckerService::IcdCheck(std::string strConfigFilePath, std::string strFilePath, std::string strResultStorePath, std::string &strResultJson)
{
    //�жϲ���ϵͳ���ͣ���linuxĿ¼�ṹת����windowsĿ¼�ṹ
    if (g_System->IsRunningOnWine())
    {
        strConfigFilePath =  g_System->replace_all_distinct(strConfigFilePath,"/","\\");
        string ss = "Z:";
        ss.append(strConfigFilePath);
        strConfigFilePath =ss;

        strFilePath =  g_System->replace_all_distinct(strFilePath,"/","\\");
        string ss2 = "Z:";
        ss2.append(strFilePath);
        strFilePath =ss2;

        strResultStorePath =  g_System->replace_all_distinct(strResultStorePath,"/","\\");
        string ss3 = "Z:";
        ss3.append(strResultStorePath);
        strResultStorePath =ss3;
    }

    std::string oldstr = "/";
    std::string newstr = "\\";
    Replace(strConfigFilePath, oldstr, newstr);
    Replace(strFilePath, oldstr, newstr);
    Replace(strResultStorePath, oldstr, newstr);
    strResultJson = CScdIcdCheckService_CheckerInvoker::GetInstance()->InvokeIcdChecker(strConfigFilePath, strFilePath, strResultStorePath);
    return SOAP_OK;
}

int ScdIcdCheckerService::ScdCrcCalc(std::string strFilePath, std::string strResultStorePath, std::string &strResultJson)
{
    //�жϲ���ϵͳ���ͣ���linuxĿ¼�ṹת����windowsĿ¼�ṹ
    if (g_System->IsRunningOnWine())
    {
        strFilePath =  g_System->replace_all_distinct(strFilePath,"/","\\");
        string ss2 = "Z:";
        ss2.append(strFilePath);
        strFilePath =ss2;

        strResultStorePath =  g_System->replace_all_distinct(strResultStorePath,"/","\\");
        string ss3 = "Z:";
        ss3.append(strResultStorePath);
        strResultStorePath =ss3;
    }

    std::string oldstr = "/";
    std::string newstr = "\\";
    Replace(strFilePath, oldstr, newstr);
    Replace(strResultStorePath, oldstr, newstr);
    strResultJson = CScdIcdCheckService_ScdCalcCrcInvoker::GetInstance()->InvokeScdCalcCrc(strFilePath, strResultStorePath);
    return SOAP_OK;
}
