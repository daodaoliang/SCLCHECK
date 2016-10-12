#ifndef _SCDICDCHECKSERVICE_SCDCALCCRCINVOKER_H
#define _SCDICDCHECKSERVICE_SCDCALCCRCINVOKER_H

#include "../ScdCheckCommon/Common.h"
#include <Windows.h>

class CScdIcdCheckService_ScdCalcCrcInvoker : public CSingleInstance<CScdIcdCheckService_ScdCalcCrcInvoker>, CNoCopy
{
public:
    CScdIcdCheckService_ScdCalcCrcInvoker();
    virtual ~CScdIcdCheckService_ScdCalcCrcInvoker();

public:
    std::string InvokeScdCalcCrc(const std::string &strFilePath, const std::string &strResultStorePath) const;

private:
    std::string m_strCalcCrcDllPath;
};

#endif /* _SCDICDCHECKSERVICE_SCDCALCCRCINVOKER_H */