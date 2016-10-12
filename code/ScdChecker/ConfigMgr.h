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

    // ȡ������Ŀ����Ϣ�� ����Ŀ��NameΪKey
    CheckItemConfig GetSubItemConfig(const std::string &strName) const;

    bool IsInSubstationTypeMap(const std::string & type);
    bool IsInBayTypeMap(const std::string & type);
    bool IsInEquipmentTypeMap(const std::string & type);
    int IsInVoltageLevelTypeMap(const std::string & type);

    bool IsSignalValidSet(){
        return m_SignalValidSet;
    }

private:
    std::map<std::string, CheckItemConfig> m_configs;
    std::map<std::string, CheckItemConfig> m_SubItemConfigs; // �����Լ������洢,
    bool m_SignalValidSet;

    // SSD��׼�����ļ�
    char lpFilename[256];
    bool ParseStandardConfigFile();
    std::map<std::string, std::string> m_SubstationTypeMap;
    std::map<std::string, std::string> m_BayTypeMap;
    std::map<std::string, std::string> m_EquipmentTypeMap;
    std::map<std::string, int> m_VoltageLevelTypeMap;
};

#endif /* _CONFIGMGR_H */