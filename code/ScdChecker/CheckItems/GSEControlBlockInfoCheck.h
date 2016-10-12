/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: AE901A
 * Name: GSEControlBlockInfoCheck
 * Desc: GSE���ƿ��������GOID��appID����APPID��MAC��ַ��VLAN-ID�����ȼ���MaxTime��MinTime����ȱʧ��
 * Ref: GDW1396
 */

#ifndef _GSEControlBlockInfoCheck_H
#define _GSEControlBlockInfoCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"

class CGSEControlBlockInfoCheck : public CSingleInstance<CGSEControlBlockInfoCheck>, CNoCopy, public CCheckItemBase
{
public:
    CGSEControlBlockInfoCheck();

public:
    virtual bool CheckInternal();
};

#endif /* _GSEControlBlockInfoCheck_H */
