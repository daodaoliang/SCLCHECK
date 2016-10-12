#include "OutputMgr.h"
#include <Shlwapi.h>
#include <stdarg.h>
#include <stdio.h>
#include <vector>
#include "../ScdCheckCommon/charconv.h"

using namespace std;

COutputMgr::COutputMgr()
    : m_ocOutput(OC_ANSI)
    , m_ocError(OC_ANSI)
{
}

COutputMgr::~COutputMgr()
{
}

void COutputMgr::ChangeOutputMode(OutputHandle oh, OutputCharset oc)
{
    if (oh == OH_OUTPUT)
    {
        m_ocOutput = oc;
    }
    else if (oh == OH_ERROR)
    {
        m_ocError = oc;
    }
    else
    {
        // error
    }
}

OutputCharset COutputMgr::GetOutputMode(OutputHandle oh) const
{
    if (oh == OH_OUTPUT)
    {
        return m_ocOutput;
    }
    else if (oh == OH_ERROR)
    {
        return m_ocError;
    }
    else
    {
        return OC_ANSI;
    }
}

static int vfprintfUtf8(FILE *file, const char *fmt, va_list args)
{
    char szBuffer[1024 * 10 * 40];

    szBuffer[0] = '\0';
    szBuffer[sizeof(szBuffer) - 1] = '\0';
    szBuffer[sizeof(szBuffer) - 2] = '\0';
    _vsnprintf(szBuffer, sizeof(szBuffer) - 1, fmt, args);

    return fprintf(file, "%s", AtoU(szBuffer).c_str());
}

int COutputMgr::Output(const char *fmt, ...)
{
    int nResult = 0;
    va_list val;

    va_start(val, fmt);

    if (m_ocOutput == OC_ANSI)
    {
        nResult = vfprintf(stdout, fmt, val);
    }
    else
    {
        nResult = vfprintfUtf8(stdout, fmt, val);
    }

    va_end(val);

    return nResult;
}

int COutputMgr::Error(const char *fmt, ...)
{
    int nResult = 0;
    va_list val;

    va_start(val, fmt);

    if (m_ocError == OC_ANSI)
    {
        nResult = vfprintf(stderr, fmt, val);
    }
    else
    {
        nResult = vfprintfUtf8(stderr, fmt, val);
    }

    va_end(val);
    FlushFileBuffers(GetStdHandle(STD_ERROR_HANDLE));

    return nResult;
}