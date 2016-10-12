/*
 *  @file  : ScdCrcCalculator.cpp
 *  @author: luteng
 *  @date  : 2015-07-14 15:16:11.343
 *  @note  : Generated by SlxTemplates
 */

#include <Windows.h>
#include <Shlwapi.h>
#include "ScdCrcCalculator.h"
#include "ScdCrcCalculatorImpl.h"
#include <stdio.h>
#include "../ScdCheckCommon/charconv.h"
#include <ShlObj.h>
#include <cassert>
#include <fstream>

using namespace std;

HINSTANCE g_hinstDll = NULL;

// DllMain
int APIENTRY DllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        g_hinstDll = hinstDll;
        DisableThreadLibraryCalls(hinstDll);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {

    }
    else
    {
        // we should not be here
    }

    return TRUE;
}

static bool DumpStringToFile(const string &str, const string &strFilePath)
{
    try
    {
        ofstream ofs;

        ofs.open(strFilePath.c_str());
        ofs<<str;
        ofs.close();

        return true;
    }
    catch (...)
    {
    }

    return false;
}

void ReplaceQuotationMark(WCHAR *dest, LPCWSTR path, int size)
{
    for (int i = 0; i <= size; ++i)
    {
        WCHAR ch = path[i];

        if (ch == L'\"')
        {
            ch = L'\0';
        }

        dest[i] = ch;

        if (ch == L'\0')
        {
            break;
        }
    }

}

// rundll32 .dll CalcCrcByRundll32 lpFilePath lpResultStorePath
// �ɹ�������result.xml��progress.txt
// ʧ��������progress.txt
void WINAPI CalcCrcByRundll32W(HWND hwndStub, HINSTANCE hInstance, LPCWSTR lpCmdLine, int nShowCmd)
{
    // 
    int nArgC = 0;
    LPWSTR *lpArgV = CommandLineToArgvW(lpCmdLine, &nArgC);     // let lpArgV leak

    if (nArgC < 2 || lpArgV == NULL)
    {
        return;
    }

    // 
    LPCWSTR lpFilePath = lpArgV[0];
    LPCWSTR lpWorkPath = lpArgV[1];

    WCHAR szFilePath[MAX_PATH];
    WCHAR szWorkPath[MAX_PATH];
    ReplaceQuotationMark(szFilePath, lpFilePath, MAX_PATH);
    ReplaceQuotationMark(szWorkPath, lpWorkPath, MAX_PATH);

    int ret = SHCreateDirectory(NULL, szWorkPath);
    if (ERROR_SUCCESS != ret &&
        ERROR_ALREADY_EXISTS != ret &&
        ERROR_FILE_EXISTS != ret)
    {
        OutputDebugString(L"Create Dir  Error!!!!!!!!!!!!!!!!!!!!!!");
        return;
    }
    OutputDebugString(L"Create Dir  OK!!!!!!!!!!!!!!!!!!!!!!");

    WCHAR szResultFile[MAX_PATH], szProgressFile[MAX_PATH];
    lstrcpyW(szResultFile, szWorkPath);
    lstrcpyW(szProgressFile, szWorkPath);

    PathAppend(szResultFile, L"\\result.xml");
    PathAppend(szProgressFile, L"progress.txt");

    char szErrorMessageUtf8[1024];
    LPCSTR lpText = CalcCrc(WtoA(lpArgV[0]).c_str(), szErrorMessageUtf8, sizeof(szErrorMessageUtf8));
    OutputDebugString(L"CalcCrc  Over!!!!!!!!!!!!!!!!!!!!!!");
    if (lpText != NULL && DumpStringToFile(lpText, WtoA(szResultFile)))
    {
        DumpStringToFile("ok", WtoA(szProgressFile));
    }
    else
    {
        DumpStringToFile(szErrorMessageUtf8, WtoA(szProgressFile));
    }
}

void WINAPI CalcCrcFreeMemory(LPCSTR lpText)
{
    free(const_cast<char *>(lpText));
}

LPCSTR WINAPI CalcCrc(LPCSTR lpFilePath, char szErrorMessage[], int nBufferSize)
{
    // 
    assert(nBufferSize > 0);
    szErrorMessage[0] = '\0';

    // 
    WIN32_FILE_ATTRIBUTE_DATA wfad;

    if (!GetFileAttributesExA(lpFilePath, GetFileExInfoStandard, &wfad) || 0 != (wfad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        wnsprintfA(szErrorMessage, nBufferSize, WtoU(L"%hs���ǺϷ����ļ�").c_str(), lpFilePath);
        return NULL;
    }

    // 
    return CalcCrcImpl(lpFilePath, szErrorMessage, nBufferSize);
}