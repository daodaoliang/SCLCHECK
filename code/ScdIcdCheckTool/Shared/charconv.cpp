/*
 *  @file  : charconv.cpp
 *  @author: 陆腾
 *  @date  : 2013/07/22
 *  @note  : 定义四种字符串类型，并提供互转换接口的实现
 */

#include "charconv.h"
#include <Windows.h>
#include <tchar.h>

using namespace std;

strutf8 ToUtf8A(const string &str)
{
    return ToUtf8W(ToWideChar(str));
}

strutf8 ToUtf8W(const wstring &str)
{
    strutf8 ret;

    int count = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

    if (count > 0)
    {
        char *buffer = (char *)malloc(count);

        if (buffer != 0)
        {
            WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, buffer, count, NULL, NULL);
            ret = buffer;

            free((void *)buffer);
        }
    }

    return ret;
}

string ToCommonA(const strutf8 &str)
{
    return ToMultiByte(ToCommonW(str));
}

wstring ToCommonW(const strutf8 &str)
{
    wstring ret;

    int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

    if (count > 0)
    {
        wchar_t *buffer = (wchar_t *)malloc(count * sizeof(wchar_t));

        if (buffer != 0)
        {
            MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, count);
            ret = buffer;

            free((void *)buffer);
        }
    }

    return ret;
}

string ToMultiByte(const wstring &str)
{
    string ret;

    int count = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

    if (count > 0)
    {
        char *buffer = (char *)malloc(count);

        if (buffer != 0)
        {
            WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, buffer, count, NULL, NULL);
            ret = buffer;

            free((void *)buffer);
        }
    }

    return ret;
}

wstring ToWideChar(const string &str)
{
    wstring ret;

    int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

    if (count > 0)
    {
        wchar_t *buffer = (wchar_t *)malloc(count * sizeof(wchar_t));

        if (buffer != 0)
        {
            MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, count);
            ret = buffer;

            free((void *)buffer);
        }
    }

    return ret;
}
