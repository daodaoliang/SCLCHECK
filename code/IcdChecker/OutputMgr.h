#ifndef _OUTPUTMGR_H
#define _OUTPUTMGR_H

#include "../ScdCheckCommon/Common.h"

enum OutputHandle
{
    OH_OUTPUT,      // 标准输出
    OH_ERROR,       // 错误输出
};

enum OutputCharset
{
    OC_ANSI,        // ANSI字符集
    OC_UTF8,        // Utf8字符集
};

class COutputMgr : public CSingleInstance<COutputMgr>, CNoCopy
{
public:
    COutputMgr();
    virtual ~COutputMgr();

    void ChangeOutputMode(OutputHandle oh, OutputCharset oc);
    OutputCharset GetOutputMode(OutputHandle oh) const;

public:
    int Output(const char *fmt, ...);
    int Error(const char *fmt, ...);

private:
    OutputCharset m_ocOutput;
    OutputCharset m_ocError;
};

#endif /* _OUTPUTMGR_H */