#ifndef _CONFIGMGR_H
#define _CONFIGMGR_H

#include <string>
#include <map>
#include "../ScdCheckCommon/Common.h"

enum ErrorType
{
    ET_WARNING,                            // 警告
    ET_ERROR,                              // 错误
    ET_TIP,                                // 提示
};

struct CheckItemConfig
{
    std::string strCheckClassKey;          // 检测项所属类别的key
    std::string strCheckClassDesc;         // 检测项所属类别的描述
    std::string strCheckItemKey;           // 检测项的key
    std::string strCheckItemName;          // 检测项的name
    std::string strCheckItemDesc;          // 检测项的描述
    std::string strCheckItemRef;           // 检测项的参考标准
    bool bEnabled;                         // 检测项是否启用
    ErrorType etValue;                     // 检测项的错误级别
    std::string strEtc1;                   // 检测项中可扩展的其他配置1
    std::string strEtc2;                   // 检测项中可扩展的其他配置2
    std::string strEtc3;                   // 检测项中可扩展的其他配置3
    std::string strEtc4;                   // 检测项中可扩展的其他配置4

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