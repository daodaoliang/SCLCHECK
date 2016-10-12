#ifndef _OUTPUTMGR_H
#define _OUTPUTMGR_H

#include "../ScdCheckCommon/Common.h"

enum OutputHandle
{
    OH_OUTPUT,      // ��׼���
    OH_ERROR,       // �������
};

enum OutputCharset
{
    OC_ANSI,        // ANSI�ַ���
    OC_UTF8,        // Utf8�ַ���
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