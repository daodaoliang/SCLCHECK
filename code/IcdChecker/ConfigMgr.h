#ifndef _CONFIGMGR_H
#define _CONFIGMGR_H

#include <string>
#include <map>
#include "../ScdCheckCommon/Common.h"

enum ErrorType
{
    ET_WARNING,                            // ����
    ET_ERROR,                              // ����
    ET_TIP,                                // ��ʾ
};

struct CheckItemConfig
{
    std::string strCheckClassKey;          // �������������key
    std::string strCheckClassDesc;         // �����������������
    std::string strCheckItemKey;           // ������key
    std::string strCheckItemName;          // ������name
    std::string strCheckItemDesc;          // ����������
    std::string strCheckItemRef;           // �����Ĳο���׼
    bool bEnabled;                         // ������Ƿ�����
    ErrorType etValue;                     // �����Ĵ��󼶱�
    std::string strEtc1;                   // ������п���չ����������1
    std::string strEtc2;                   // ������п���չ����������2
    std::string strEtc3;                   // ������п���չ����������3
    std::string strEtc4;                   // ������п���չ����������4

public:
    CheckItemConfig();
};

class CConfigMgr : public CSingleInstance<CConfigMgr>, CNoCopy
{
public:
    CConfigMgr();
    virtual ~CConfigMgr();

    bool SetConfigFilePath(const char *lpFilePath);
    bool GenerateFiles() const;
    bool GetItemConfig(const std::string &strKey, CheckItemConfig &cfg) const;

private:
    std::map<std::string, CheckItemConfig> m_configs;
};

#endif /* _CONFIGMGR_H */