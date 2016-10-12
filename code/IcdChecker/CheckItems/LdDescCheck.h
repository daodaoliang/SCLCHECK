/* �˶�������ȡ��Ĭ�������ļ�����ʵ����������г���
 * 
 * Key: 661141
 * Name: LdDescCheck
 * Desc: LD��LNʵ���ġ�desc������ֵ�Ƿ�ǿ�
 * Ref: Q/GDW 1396-2012 ��6.2��
 */

#ifndef _LdDescCheck_H
#define _LdDescCheck_H

#include "../../ScdCheckCommon/Common.h"
#include "../CheckItem.h"
#include "../CacheMgr.h"

class CLdDescCheck : public CSingleInstance<CLdDescCheck>, CNoCopy, public CCheckItemBase
{
public:
    CLdDescCheck();

public:
    virtual bool CheckInternal();

    void CheckLNDesc_InCacheMgr(std::wstring sDesc, int offset);
    void CheckLDeviceDesc_InCacheMgr(LDeviceCache& device, int offset);
};

#endif /* _LdDescCheck_H */
