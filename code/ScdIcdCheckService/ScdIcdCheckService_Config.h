#ifndef _SCDICDCHECKSERVICE_CONFIG_H
#define _SCDICDCHECKSERVICE_CONFIG_H

#include "../ScdCheckCommon/Common.h"
#include <string>
#include <vector>

class CScdIcdCheckService_Config : CNoCopy, public CSingleInstance<CScdIcdCheckService_Config>
{
    friend CSingleInstance<CScdIcdCheckService_Config>;

private:
    CScdIcdCheckService_Config();
    ~CScdIcdCheckService_Config();

public:
    bool Load();
    bool Save() const;

    int GetThreadCount() const;
    int GetPortID() const;

private:
    int m_nThreadCount;
    int m_nPortID;
    std::vector<std::string> m_vectorFilePaths;
    mutable std::string m_strConfigFilePath;

private:
    void InitPaths();
};

#endif /* _SCDICDCHECKSERVICE_CONFIG_H */