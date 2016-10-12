#include "Common.h"
#include <Windows.h>
#include <Shlwapi.h>
#include <sstream>
#include <strsafe.h>

#pragma comment(lib, "Shlwapi.lib")

using namespace std;

#define REG_BASE_PATH   "Software\\Roof\\SclDac"
#define REG_PATH        "Software\\Roof\\SclDac\\Temp\\LastError"

static DWORD SHSetTempValueA(HKEY hRootKey, LPCSTR pszSubKey, LPCSTR pszValue, DWORD dwType, LPCVOID pvData, DWORD cbData)
{
    HKEY hKey = NULL;
    DWORD dwRet = RegCreateKeyExA(hRootKey, pszSubKey, 0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);

    if(ERROR_SUCCESS == dwRet)
    {
        dwRet = RegSetValueExA(hKey, pszValue, 0, dwType, (const unsigned char *)pvData, cbData);
        RegCloseKey(hKey);
    }

    return dwRet;
}

void CLastError::Get(std::string &strLastErrorMsg)
{
    char szBuffer[8192] = "";
    DWORD dwSize = sizeof(szBuffer) - 1;
    string strValue = CommonType2String(GetCurrentThreadId());

    SHGetValueA(HKEY_CURRENT_USER, REG_PATH, strValue.c_str(), NULL, szBuffer, &dwSize);
    strLastErrorMsg = szBuffer;
}

void CLastError::Set(const std::string &strLastErrorMsg)
{
    string strValue = CommonType2String(GetCurrentThreadId());
    string strData = GetNowTimeString() + " - " + strLastErrorMsg;

    SHSetValueA(HKEY_CURRENT_USER, REG_BASE_PATH, NULL, REG_SZ, NULL, 0);
    SHSetTempValueA(HKEY_CURRENT_USER, REG_PATH, strValue.c_str(), REG_SZ, strData.c_str(), strData.length() + 1);
}

std::string GetFileSizeString(const std::string &strFilePath)
{
    WIN32_FILE_ATTRIBUTE_DATA wfad;
    ULARGE_INTEGER uliFileSize = {0};

    if (GetFileAttributesExA(strFilePath.c_str(), GetFileExInfoStandard, &wfad))
    {
        uliFileSize.LowPart = wfad.nFileSizeLow;
        uliFileSize.HighPart = wfad.nFileSizeHigh;
    }

    return CommonType2String(uliFileSize.QuadPart);
}

static std::string GetTimeString(const SYSTEMTIME *pSt)
{
    char szTimeString[128] = "";

    StringCchPrintfA(
        szTimeString,
        RTL_NUMBER_OF(szTimeString),
        "%04u-%02u-%02u %02u:%02u:%02u.%03u",
        pSt->wYear,
        pSt->wMonth,
        pSt->wDay,
        pSt->wHour,
        pSt->wMinute,
        pSt->wSecond,
        pSt->wMilliseconds);

    return szTimeString;
}

std::string GetFileModifyTimeString(const std::string &strFilePath)
{
    HANDLE hFile = CreateFileA(strFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        FILETIME ft, ftLocal;
        SYSTEMTIME st;

        GetFileTime(hFile, NULL, NULL, &ft);
        CloseHandle(hFile);

        FileTimeToLocalFileTime(&ft, &ftLocal);
        FileTimeToSystemTime(&ftLocal, &st);

        return GetTimeString(&st);
    }

    return "";
}

std::string GetNowTimeString()
{
    SYSTEMTIME st;

    GetLocalTime(&st);

    return GetTimeString(&st);
}

bool DeleteOrRenameFile(const std::string &strFilePath)
{
    if (DeleteFileA(strFilePath.c_str()))
    {
        return true;
    }

    return !!MoveFileA(strFilePath.c_str(), (strFilePath + ".D" + CommonType2String(GetTickCount())).c_str());
}

std::string XmlEscape(const std::string &strText)
{
    stringstream ss;

    for (string::const_iterator it = strText.begin(); it != strText.end(); ++it)
    {
        char ch = *it;

        switch (ch)
        {
        case '&':
            ss<<"&amp;";
            break;

        case '<':
            ss<<"&lt;";
            break;

        case '>':
            ss<<"&gt;";
            break;

        case '\'':
            ss<<"&apos;";
            break;

        case '\"':
            ss<<"&quot;";
            break;

        default:
            ss<<ch;
            break;
        }
    }

    return ss.str();
}

std::string FormatXMLMessage(const char *fmt, ...)
{
    char szText[10 * 1024] = "";
    va_list val;

    va_start(val, fmt);
    _vsnprintf(szText, sizeof(szText), fmt, val);
    va_end(val);

    return XmlEscape(szText);

}

int GetSmallFileSize(const std::string &strFilePath)
{
    WIN32_FILE_ATTRIBUTE_DATA wfad;

    if (GetFileAttributesExA(strFilePath.c_str(), GetFileExInfoStandard, &wfad))
    {
        if (wfad.nFileSizeHigh == 0)
        {
            return wfad.nFileSizeLow;
        }
    }

    return -1;
}

void SafeDebugMessageA(LPCSTR pFormat, ...)
{
    char szBuffer[4096];
    va_list pArg;

    va_start(pArg, pFormat);
    StringCchVPrintfA(szBuffer, sizeof(szBuffer), pFormat, pArg);
    va_end(pArg);

    OutputDebugStringA(szBuffer);
}

std::string MakeResultJson(bool bSuccess, const char *fmt, ...)
{
    char szText[8192 + 1024] = "";

    if (bSuccess)
    {
        lstrcpynA(szText, "{\"status\" = \"success\"}", RTL_NUMBER_OF(szText));
    }
    else
    {
        va_list val;
        char szMessage[8192] = "";

        va_start(val, fmt);
        StringCchVPrintfA(szMessage, RTL_NUMBER_OF(szMessage), fmt, val);
        va_end(val);
        
        StringCchPrintfA(szText, RTL_NUMBER_OF(szText), "{\"status\" = \"failed\", \"message\" = \"%s\"}", szMessage);
    }

    return szText;
}

bool IsSystem64bit()
{
#ifdef _WIN64
    return true;
#else
    BOOL (WINAPI *pIsWow64Process)(
        __in          HANDLE hProcess,
        __out         PBOOL Wow64Process
        ) = NULL;

    (PROC &)pIsWow64Process = GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "IsWow64Process");

    if (pIsWow64Process != NULL)
    {
        BOOL bIsWow64Process = FALSE;

        if (pIsWow64Process(GetCurrentProcess(), &bIsWow64Process))
        {
            return !!bIsWow64Process;
        }
    }

    return false;
#endif
}

vector<std::wstring> split_string(const std::wstring& sSource, const std::wstring& sDelim, bool ignore_blank)
{    
    vector<wstring> vecDest;

    wstring::size_type pos = sSource.find(sDelim);
    wstring::size_type begin = 0;
    bool first = true;

    while (true)
    {
        if (pos == sSource.npos)
        {
            if (begin < sSource.size())
            {
                vecDest.push_back(sSource.substr(begin));
            }

            return vecDest;
        }
        else
        {
            if (pos > begin || (!first && !ignore_blank))
            {
                vecDest.push_back(sSource.substr(begin, pos - begin));
            }

            begin = pos + sDelim.size();
            pos = sSource.find(sDelim, begin);

            first = false;
        }
    }

    return vecDest;
}
/*
    补全文件路径
    sPathIn:传入的相对路径
    sPathOut:传出的绝对路径
    若传入的路径不存在，返回false
    author:cxl
*/
bool ResolveFilePath(wstring sPathIn,wstring& sPathOut)
{
    // 同级目录下查找
    wchar_t szModulePath[MAX_PATH] = {0};
    GetModuleFileNameW(NULL,szModulePath,MAX_PATH);
    PathRemoveFileSpecW(szModulePath);
    PathAppendW(szModulePath,sPathIn.c_str());
    if (PathFileExistsW(szModulePath))
    {
        sPathOut = szModulePath;
        return TRUE;
    }
    // 同级 x64下查找
    GetModuleFileNameW(NULL,szModulePath,MAX_PATH);
    PathRemoveFileSpecW(szModulePath);
    PathAppendW(szModulePath, L"x64");
    PathAppendW(szModulePath,sPathIn.c_str());
    if (PathFileExistsW(szModulePath))
    {
        sPathOut = szModulePath;
        return TRUE;
    }
    // 父目录下查找
    GetModuleFileNameW(NULL,szModulePath,MAX_PATH);
    PathRemoveFileSpecW(szModulePath);
    PathAppendW(szModulePath, L"..");
    PathAppendW(szModulePath,sPathIn.c_str());
    if (PathFileExistsW(szModulePath))
    {
        sPathOut = szModulePath;
        return TRUE;
    }
    return false;
}