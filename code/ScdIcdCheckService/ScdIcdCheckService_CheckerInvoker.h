#ifndef _SCDICDCHECKSERVICE_CHECKERINVOKER_H
#define _SCDICDCHECKSERVICE_CHECKERINVOKER_H

#include "../ScdCheckCommon/Common.h"
#include "../ScdCheckCommon/charconv.h"

class CScdIcdCheckService_CheckerInvoker : public CSingleInstance<CScdIcdCheckService_CheckerInvoker>, CNoCopy
{
public:
    CScdIcdCheckService_CheckerInvoker();
    virtual ~CScdIcdCheckService_CheckerInvoker();

public:
    std::strutf8 InvokeScdChecker(const std::string &strConfigFilePath, const std::string &strFilePath, const std::string &strResultStorePath) const;
    std::strutf8 InvokeIcdChecker(const std::string &strConfigFilePath, const std::string &strFilePath, const std::string &strResultStorePath) const;

private:
    std::strutf8 InvodeChecker(
        const std::string &strCheckerPath,
        const std::string &strConfigFilePath,
        const std::string &strFilePath,
        const std::string &strResultStorePath) const;
    bool ConvertResultXmlToRestltXslx(
        const std::string &strFilePath,                     // scd或icd文件路径
        const std::string &strResultXmlFilePath,            // 结果xml文件
        const std::string &strResultXlsxFilePath) const;    // 要生成的结果xlsx文件

private:
    std::string m_strScdCheckerPath;
    std::string m_strIcdCheckerPath;
    std::string m_strXml2XslPath;
};

#endif /* _SCDICDCHECKSERVICE_CHECKERINVOKER_H */