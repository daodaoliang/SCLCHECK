/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 9A2FE1
 * Name: ServerCheck
 * Desc: Server����������Ӧ����һ��LDevice����ÿ��LDevice��������Ӧ����LLN0��LPHD�����߼��ڵ㡣
 * Ref: Q/GDW 1396-2012 ��7.1.1��
 */

#ifndef _ServerCheck_H
#define _ServerCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CServerCheck : public CSingleInstance<CServerCheck>, CNoCopy, public CCheckItemBase
{
public:
    CServerCheck();

public:
    virtual bool CheckInternal();

    void CheckLLN0LPHD_InCacheMgr(LDeviceCache& lDevice);
    void CheckhasLDevice_InCacheMgr(bool bHasLDevice, int offset);
};

#endif /* _ServerCheck_H */
