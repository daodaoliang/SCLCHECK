#ifndef _CHECKITEM_H
#define _CHECKITEM_H

#include <string>
#include "ConfigMgr.h"

class CCheckItemBase
{
protected:
    // 构造
    CCheckItemBase();

public:
    // 析构
    virtual ~CCheckItemBase();

public:
    // 获取当前检测项的配置项信息
    const CheckItemConfig &GetItemConfig() const;

    // 判断当前检测项是否被启用
    bool IsEnabled() const;

    // 得到当前检测项的错误类型（提示、警告、错误）
    ErrorType GetErrorType() const;

    // 得到当前检测项的错误类型，字符串形式
    std::string GetErrorTypeString() const;

    // 开始检测，如果当前检测项被启用，会调用CheckInternal做实际的检测
    bool Check();

    // 汇报，包括当前行、xml片段和具体错误描述信息
    // strXmlText要求输入为utf8格式
    // 具体错误信息要求输入为ansi格式
    // Report最终汇报为xml格式，strXmlText和错误信息都会在内部被自动xml转义
    bool Report(int nRow, const std::string &strXmlText, const char *fmt, ...) const;

protected:
    // 实际的检测动作，子类必须实现，返回是否成功
    // 检测完毕正常结束即为成功（无论汇报了多少错误和警告信息）
    virtual bool CheckInternal() = 0;

    // 私有函数，设定当前检测项的配置项
    void SetConfig(const CheckItemConfig &config);

private:
    // 当前检测项的配置项
    CheckItemConfig m_config;
};

#endif /* _CHECKITEM_H */