#ifndef _CHECKITEM_H
#define _CHECKITEM_H

#include <string>
#include "ConfigMgr.h"

class CCheckItemBase
{
protected:
    // ����
    CCheckItemBase();

public:
    // ����
    virtual ~CCheckItemBase();

public:
    // ��ȡ��ǰ��������������Ϣ
    const CheckItemConfig &GetItemConfig() const;

    // �жϵ�ǰ������Ƿ�����
    bool IsEnabled() const;

    // �õ���ǰ�����Ĵ������ͣ���ʾ�����桢����
    ErrorType GetErrorType() const;

    // �õ���ǰ�����Ĵ������ͣ��ַ�����ʽ
    std::string GetErrorTypeString() const;

    // ��ʼ��⣬�����ǰ�������ã������CheckInternal��ʵ�ʵļ��
    bool Check();

    // �㱨��������ǰ�С�xmlƬ�κ;������������Ϣ
    // strXmlTextҪ������Ϊutf8��ʽ
    // ���������ϢҪ������Ϊansi��ʽ
    // Report���ջ㱨Ϊxml��ʽ��strXmlText�ʹ�����Ϣ�������ڲ����Զ�xmlת��
    bool Report(int nRow, const std::string &strXmlText, const char *fmt, ...) const;

protected:
    // ʵ�ʵļ�⶯�����������ʵ�֣������Ƿ�ɹ�
    // ����������������Ϊ�ɹ������ۻ㱨�˶��ٴ���;�����Ϣ��
    virtual bool CheckInternal() = 0;

    // ˽�к������趨��ǰ������������
    void SetConfig(const CheckItemConfig &config);

private:
    // ��ǰ������������
    CheckItemConfig m_config;
};

#endif /* _CHECKITEM_H */