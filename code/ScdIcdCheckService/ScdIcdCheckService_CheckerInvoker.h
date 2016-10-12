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
        const std::string &strFilePath,                     // scd��icd�ļ�·��
        const std::string &strResultXmlFilePath,            // ���xml�ļ�
        const std::string &strResultXlsxFilePath) const;    // Ҫ���ɵĽ��xlsx�ļ�

private:
    std::string m_strScdCheckerPath;
    std::string m_strIcdCheckerPath;
    std::string m_strXml2XslPath;
};

#endif /* _SCDICDCHECKSERVICE_CHECKERINVOKER_H */